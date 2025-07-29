#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>

void FitCRC_Get16(uint16_t& crc, uint8_t byte)
{
	static const uint16_t crc_table[16] =
	{
		0x0000, 0xCC01, 0xD801, 0x1400, 0xF001, 0x3C00, 0x2800, 0xE401,
		0xA001, 0x6C00, 0x7800, 0xB401, 0x5000, 0x9C01, 0x8801, 0x4400
	};
	uint16_t tmp;
	// compute checksum of lower four bits of byte
	tmp = crc_table[crc & 0xF];
	crc = (crc >> 4) & 0x0FFF;
	crc = crc ^ tmp ^ crc_table[byte & 0xF];
	// now compute checksum of upper four bits of byte
	tmp = crc_table[crc & 0xF];
	crc = (crc >> 4) & 0x0FFF;
	crc = crc ^ tmp ^ crc_table[(byte >> 4) & 0xF];
}

struct definition_message {
	uint16_t global_message_number_;
	uint8_t num_field_;
	std::vector<std::vector<uint8_t>> fields_;
	uint8_t local_message_type_;

	definition_message(	uint16_t gmn,
						uint8_t num_field,
						uint8_t lmt) :	global_message_number_(gmn),
										num_field_(num_field),
										local_message_type_(lmt) {}

	void add_field(uint8_t num, uint8_t size, uint8_t type) {
		std::vector<uint8_t> field {num, size, type};
		fields_.push_back(field);
	}
};

bool check_crc_beg(std::istream& is) {
	uint16_t crc=0;
	is.seekg(0, std::ios::beg);
	for (uint8_t i=0; i<12; ++i) {
		FitCRC_Get16(crc, is.get());
	}
	uint16_t crc_read;
	is.read(reinterpret_cast<char*>(&crc_read), 2);
	is.seekg(0, std::ios::beg);
	if (crc == crc_read) { return true; }
	else { return false; }
}

bool check_crc_end(const std::string& filename) {
	std::ifstream is(filename, std::ios::binary);
	uint16_t crc = 0;
	is.seekg(0, std::ios::end);
	size_t file_size = is.tellg();
	is.seekg(0, std::ios::beg);
	for (uint8_t i=0; i<14; ++i) {
		is.get();
	}
	for (size_t i=14; i<file_size; ++i) {
		FitCRC_Get16(crc, is.get());
	}
	uint16_t crc_end;
	is.read(reinterpret_cast<char*>(&crc_end), 2);
	if (crc_end == crc) { return true; }
	else { return false; }
}

bool read_header(std::istream& is, uint32_t& data_size) {
	if (!check_crc_beg(is)) { return false; }
    uint8_t header_size;
	uint16_t crc;
    is.read(reinterpret_cast<char *>(&header_size), 1);
	FitCRC_Get16(crc, header_size);
    uint8_t c;
	uint16_t crc_read=0;
    for (uint8_t i=1; i<header_size; ++i) {
        if (i==4) {
            is.read(reinterpret_cast<char *>(&data_size), 4);
        	i+=4;
        }
        else if (i==13) {
    		is.read(reinterpret_cast<char *>(&crc_read), 2);
    		i+=2;
    	}
        else { is.read(reinterpret_cast<char *>(&c), 1); }
    }
	return true;
}

void read_data(std::istream& is, uint32_t data_size) {
	std::vector<definition_message> definitions;
	for (size_t i=14; i<data_size+14 && is.peek()!=EOF;) {
		uint8_t byte;
		byte = is.get();
		++i;
		if (byte>=64) { //definition message
			is.get(); ++i; //reserved
			is.get(); ++i; //architecture
			uint16_t gmn;
			is.read(reinterpret_cast<char*>(&gmn), 2);
			i+=2;
			uint8_t num_field = is.get();
			++i;
			definition_message m(gmn, num_field, byte&0x0F);
			for (int j=0; j<num_field; ++j) {
				uint8_t num = is.get();
				uint8_t size = is.get();
				uint8_t type = is.get();
				m.add_field(num, size, type);
			}
			i+=num_field*3;
			definitions.push_back(m);
		}
		else { //data message
			uint8_t lmt = byte & 0x0F;
			for (const auto& m : definitions) {
				if (m.local_message_type_ == lmt) {
					uint32_t val;
					for (uint8_t j=0; j<m.num_field_; ++j) {
						if (m.fields_[j][1] > 4) {
							std::string v(m.fields_[j][1], ' ');
							is.read(reinterpret_cast<char*>(&v[0]), m.fields_[j][1]);
						}
						else {
							val = 0;
							is.read(reinterpret_cast<char*>(&val), m.fields_[j][1]);
							if (m.global_message_number_==0 && m.fields_[j][0]==4) {
								std::cout << "time_created = " << val << "\n";
							}
							if (m.global_message_number_==19 && m.fields_[j][0]==13) {
								std::cout << "avg_speed = " << std::fixed
								<< std::setprecision(3) << double(val)/1000*3.6
								<< " km/h\n";
							}
							i+=m.fields_[j][1];
						}
					}
				}
			}
		}
	}
}

int main(int argc, char* argv[]) {
    if (argc != 2) {return EXIT_FAILURE;}
	uint32_t data_size = 0;
	std::ifstream is(argv[1], std::ios::binary);
	if (!is) { return EXIT_FAILURE; }
    if (!read_header(is, data_size)) { return EXIT_FAILURE; }
	read_data(is, data_size);
	if (check_crc_end(argv[1])) {
		std::cout << "File CRC ok\n";
	}
	else { return EXIT_FAILURE; }
    return EXIT_SUCCESS;
}
