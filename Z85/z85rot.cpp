#include <cstdlib>
#include <string>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <cmath>

int key_value(std::unordered_map<int, char>& map, char c) {
    for (const auto& pair : map) {
        if (pair.second == c) { return pair.first; }
    }
    return -1;
}

void rotation(std::string& text, size_t N, bool direction, std::string& rot) {
    std::unordered_map<int, char> map = {
        {0,'0'},{1,'1'},{2,'2'},{3,'3'},
        {4,'4'},{5,'5'},{6,'6'},{7,'7'},
        {8,'8'},{9,'9'},{10,'a'},{11,'b'},
        {12,'c'},{13,'d'},{14,'e'},{15,'f'},
        {16,'g'},{17,'h'},{18,'i'},{19,'j'},
        {20,'k'},{21,'l'},{22,'m'},{23,'n'},
        {24,'o'},{25,'p'},{26,'q'},{27,'r'},
        {28,'s'},{29,'t'},{30,'u'},{31,'v'},
        {32,'w'},{33,'x'},{34,'y'},{35,'z'},
        {36,'A'},{37,'B'},{38,'C'},{39,'D'},
        {40,'E'},{41,'F'},{42,'G'},{43,'H'},
        {44,'I'},{45,'J'},{46,'K'},{47,'L'},
        {48,'M'},{49,'N'},{50,'O'},{51,'P'},
        {52,'Q'},{53,'R'},{54,'S'},{55,'T'},
        {56,'U'},{57,'V'},{58,'W'},{59,'X'},
        {60,'Y'},{61,'Z'},{62,'.'},{63,'-'},
        {64,':'},{65,'+'},{66,'='},{67,'^'},
        {68,'!'},{69,'/'},{70,'*'},{71,'?'},
        {72,'&'},{73,'<'},{74,'>'},{75,'('},
        {76,')'},{77,'['},{78,']'},{79,'{'},
        {80,'}'},{81,'@'},{82,'%'},{83,'$'},
        {84,'#'}
    };
    std::string rotated_text;
    if (direction) {
        for (size_t i=0; i<text.length(); ++i) {
            int pos = key_value(map,text[i]) - int(N*i)%85;
            if (pos < 0) { pos = 85 + pos; }
            rot += (map[pos]);
        }
    }
    else {
        for (size_t i=0; i<text.length(); ++i) {
            int pos = (key_value(map,text[i]) + int(N*i))%85;
            rot += (map[pos]);
        }
    }
}

void from_decimal_to_z85(uint32_t c, std::string& s) {
    std::string z85;
    std::unordered_map<int, char> map = {
        {0,'0'},{1,'1'},{2,'2'},{3,'3'},
        {4,'4'},{5,'5'},{6,'6'},{7,'7'},
        {8,'8'},{9,'9'},{10,'a'},{11,'b'},
        {12,'c'},{13,'d'},{14,'e'},{15,'f'},
        {16,'g'},{17,'h'},{18,'i'},{19,'j'},
        {20,'k'},{21,'l'},{22,'m'},{23,'n'},
        {24,'o'},{25,'p'},{26,'q'},{27,'r'},
        {28,'s'},{29,'t'},{30,'u'},{31,'v'},
        {32,'w'},{33,'x'},{34,'y'},{35,'z'},
        {36,'A'},{37,'B'},{38,'C'},{39,'D'},
        {40,'E'},{41,'F'},{42,'G'},{43,'H'},
        {44,'I'},{45,'J'},{46,'K'},{47,'L'},
        {48,'M'},{49,'N'},{50,'O'},{51,'P'},
        {52,'Q'},{53,'R'},{54,'S'},{55,'T'},
        {56,'U'},{57,'V'},{58,'W'},{59,'X'},
        {60,'Y'},{61,'Z'},{62,'.'},{63,'-'},
        {64,':'},{65,'+'},{66,'='},{67,'^'},
        {68,'!'},{69,'/'},{70,'*'},{71,'?'},
        {72,'&'},{73,'<'},{74,'>'},{75,'('},
        {76,')'},{77,'['},{78,']'},{79,'{'},
        {80,'}'},{81,'@'},{82,'%'},{83,'$'},
        {84,'#'}
    };
    do {
        int resto = c % 85;
        c = c/85;
        z85.insert(0,&map[resto]);
    } while (c!=0);

    if (z85.length()<5) {
        for (uint8_t i=z85.length(); i<5; ++i) {
            z85.insert(0,&map[0]);
        }
    }

    s+=z85;
}

uint32_t from_z85_to_decimal(const std::string& z85) {
    std::unordered_map<int, char> map = {
        {0,'0'},{1,'1'},{2,'2'},{3,'3'},
        {4,'4'},{5,'5'},{6,'6'},{7,'7'},
        {8,'8'},{9,'9'},{10,'a'},{11,'b'},
        {12,'c'},{13,'d'},{14,'e'},{15,'f'},
        {16,'g'},{17,'h'},{18,'i'},{19,'j'},
        {20,'k'},{21,'l'},{22,'m'},{23,'n'},
        {24,'o'},{25,'p'},{26,'q'},{27,'r'},
        {28,'s'},{29,'t'},{30,'u'},{31,'v'},
        {32,'w'},{33,'x'},{34,'y'},{35,'z'},
        {36,'A'},{37,'B'},{38,'C'},{39,'D'},
        {40,'E'},{41,'F'},{42,'G'},{43,'H'},
        {44,'I'},{45,'J'},{46,'K'},{47,'L'},
        {48,'M'},{49,'N'},{50,'O'},{51,'P'},
        {52,'Q'},{53,'R'},{54,'S'},{55,'T'},
        {56,'U'},{57,'V'},{58,'W'},{59,'X'},
        {60,'Y'},{61,'Z'},{62,'.'},{63,'-'},
        {64,':'},{65,'+'},{66,'='},{67,'^'},
        {68,'!'},{69,'/'},{70,'*'},{71,'?'},
        {72,'&'},{73,'<'},{74,'>'},{75,'('},
        {76,')'},{77,'['},{78,']'},{79,'{'},
        {80,'}'},{81,'@'},{82,'%'},{83,'$'},
        {84,'#'}
    };

    uint32_t val = 0;
    for (uint8_t j=0; j<5; ++j) {
        val += key_value(map, z85[j])*pow(85,(4-j));
    }
    return val;
}

bool encode(const std::string& infile, const std::string& outfile, uint32_t N) {
    std::ifstream is(infile, std::ios::binary);
    std::string magic_number;
    if (!is) { return false; }
    uint32_t width, height, nlevels;
    is >> magic_number;
    if (magic_number != "P6") { return false; }
    is.get();
    if (is.peek() == '#') {
        std::string comment;
        std::getline(is, comment);
    }
    is >> width >> height >> nlevels;
    is.get();
    std::ofstream os(outfile);
    os << width << "," << height << ",";
    std::string z85;
    for (size_t i=0; i<width*height*3; i+=4){
        uint32_t val=0;
        for (uint8_t j=0; j<4; ++j) {
            uint8_t n = is.get();
            val = val << 8 | n;
        }
        from_decimal_to_z85(val, z85);
    }
    std::string rot;
    rotation(z85, N, true, rot);
    os << rot;

    return true;
}

bool decode(const std::string& infile, const std::string& outfile, uint32_t N) {
    std::ifstream is(infile, std::ios::binary);
    std::ofstream os(outfile, std::ios::binary);

    if (!is) { return false; }
    if (!os) { return false; }

    std::string s;
    int width, height;
    is >> width;
    is.get();
    is >> height;
    is.get();
    std::string rotated_text="";
    int pad = 0;
    if (width*height % 4 != 0) {
        pad = 4-width*height % 4;
        rotated_text.
    }
    for (size_t i=0; i < (pad + width*height)/4*5*3; ++i) {
        rotated_text += is.get();
    }
    std::string text = "";
    rotation(rotated_text, N, false, text);

    os << "P6\n" << width << " " << height << "\n255\n";
    for (size_t i=0; i<=text.length()-5; i+=5) {
        std::string sub = text.substr(i,5);
        uint32_t val = from_z85_to_decimal(sub);
        for (uint8_t k=0; k<4; ++k) {
            os.put(static_cast<uint8_t>(val/(pow(256,(3-k)))));
        }
    }

    return true;
}

int main(int argc, char* argv[]) {
    if (argc!=5) {
        return EXIT_FAILURE;
    }

    uint32_t N = std::stoi(argv[2]);

    if (argv[1][0] == 'c') {
        if (!encode(argv[3],argv[4], N)) { return EXIT_FAILURE; }
    }

    if (argv[1][0] == 'd') {
        if (!decode(argv[3], argv[4], N)) { return EXIT_FAILURE; }
    }



    return EXIT_SUCCESS;
}