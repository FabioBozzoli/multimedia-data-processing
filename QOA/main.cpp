#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <array>

struct lmsState {
    std::array<int16_t, 4> history_;
    std::array<int16_t, 4> weights_;
};

int16_t clip(const int32_t n) {
    return std::max(-32768, std::min(n, 32767));
}

void read_qoa(std::istream& is, std::vector<int16_t>& samples) {
    std::string magic(4,' ');
    uint32_t num_samples = 0;
    is.read(reinterpret_cast<char *>(&magic), 4);
    for (uint8_t i=0; i<4; ++i) num_samples = num_samples << 8 | is.get();
    while (is.peek() != EOF) {
        uint8_t num_channels = is.get();
        uint32_t samplerate = 0;
        uint16_t fsamples = 0, fsize = 0;
        for (uint8_t i=0; i<3; ++i) samplerate = samplerate << 8 | is.get();
        for (uint8_t i=0; i<2; ++i) fsamples = fsamples << 8 | is.get();
        for (uint8_t i=0; i<2; ++i) fsize = fsize << 8 | is.get();
        std::vector<lmsState> lms_state(num_channels);
        for (uint8_t i=0; i<num_channels; ++i) {
            for (uint8_t j=0; j<4; ++j) {
                for (uint8_t k=0; k<2; ++k)
                    lms_state[i].history_[j] = lms_state[i].history_[j] << 8 | is.get();
            }
            for (uint8_t j=0; j<4; ++j) {
                for (uint8_t k=0; k<2; ++k)
                    lms_state[i].weights_[j] = lms_state[i].weights_[j] << 8 | is.get();
            }
        }
        uint64_t num_slices = ceil(double(fsamples)/20.0);
        for (uint64_t i=0; i<num_slices && is.peek()!=EOF; ++i) {
            std::vector<std::vector<int16_t>> samples_channel(num_channels);
            std::vector<float> dequant_table {0.75, -0.75, 2.5, -2.5, 4.5, -4.5, 7, -7};
            for (uint8_t j=0; j<num_channels; ++j) {
                std::vector<uint8_t> slice;
                std::vector<uint8_t> residuals;
                uint8_t sf_quant = 0;
                uint64_t n = 0;
                for (uint8_t k=0; k<8 && is.peek()!=EOF; ++k) {
                    n = n << 8 | is.get();
                }
                sf_quant = (n >> 60) & 0x0F;
                for (int8_t k=1; k<21; ++k) {
                    slice.push_back((n >> (60-k*3)) & 0b00000111);
                }
                uint16_t sf = round(pow(sf_quant +1, 2.75));
                for (uint8_t k=0; k<20; ++k) {
                    double r = sf*dequant_table[slice[k]];
                    int64_t new_r;
                    if (r < 0) new_r = static_cast<int64_t>(ceil(r-0.5));
                    else new_r = static_cast<int64_t>(floor(r+0.5));
                    int64_t p=0;
                    for (uint8_t n=0; n<4; ++n) {
                        p+=lms_state[j].history_[n] * lms_state[j].weights_[n];
                    }
                    p = p >> 13;
                    int64_t s = clip(p + new_r);
                    samples_channel[j].push_back(s);
                    int64_t delta = new_r >> 4;
                    for (uint8_t n = 0; n < 4; n++) {
                        if (lms_state[j].history_[n] < 0) {
                            lms_state[j].weights_[n] += -delta;
                        }
                        else lms_state[j].weights_[n] += delta;
                    }
                    for (uint8_t n = 0; n < 3; n++) {
                        lms_state[j].history_[n] = lms_state[j].history_[n+1];
                    }
                    lms_state[j].history_[3] = s;
                }
            }
            for (uint64_t i=0; i<samples_channel[0].size(); ++i) {
                for (uint8_t j=0; j<num_channels; ++j) {
                    samples.push_back(samples_channel[j][i]);
                }
            }
        }
    }
}

void write_wav(std::ostream& os, const std::vector<int16_t>& samples) {
    os << "RIFF";
    uint64_t c = 0;
    os.write(reinterpret_cast<char*>(&c), 4);
    os << "WAVEfmt ";
    c = 16;
    os.write(reinterpret_cast<char*>(&c), 4);
    c=1;
    os.write(reinterpret_cast<char*>(&c), 2);
    c=2;
    os.write(reinterpret_cast<char*>(&c), 2);
    c = 44100;
    os.write(reinterpret_cast<char*>(&c), 4);
    c = 176400;
    os.write(reinterpret_cast<char*>(&c), 4);
    c = 4;
    os.write(reinterpret_cast<char*>(&c), 2);
    c = 16;
    os.write(reinterpret_cast<char*>(&c), 2);
    os << "data";
    c = samples.size()*2;
    os.write(reinterpret_cast<char*>(&c), 4);
    for (uint64_t i=0; i<samples.size(); ++i) {
        os.write(reinterpret_cast<const char*>(&samples[i]), 2);
    }
    uint64_t pos = os.tellp();
    pos-=8;
    os.seekp(4);
    os.write(reinterpret_cast<char*>(&pos), 4);
}

int main(int argc, char * argv[]) {
    if (argc != 3) return EXIT_FAILURE;

    std::ifstream is(argv[1], std::ios::binary);
    if (!is) return EXIT_FAILURE;
    std::ofstream os(argv[2], std::ios::binary);
    if (!os) return EXIT_FAILURE;

    std::vector<int16_t> samples;
    read_qoa(is, samples);
    write_wav(os, samples);
    return EXIT_SUCCESS;
}