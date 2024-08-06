#include <iostream>
#include <fstream>

int decodeIndex = 0;
const int decodeMovesSize = 6;
const int decodeMoves[] = { -1, 1, -2, 2, -3, +3 };

void decodeFile(std::fstream* fileI, char* filePath) {
    std::fstream fileO;
    fileO.open((std::string(filePath) + ".txt").c_str(), std::ios::out | std::ios::binary);
    
    char character = 0;
    char* buffChar = (char*)malloc(6 * sizeof(char));
    char br[] = { 0x0D, 0x0A };

    while (!fileI->eof()) {

        while (fileI->get(character)) {
            if (character == '{') {
                fileI->read(buffChar, 6);
                if (strncmp(buffChar, "<C:6>}", 6) == 0) {
                    fileO.write("{<C:6>}", 7);
                    fileO.write(br, 2);
                    continue;
                }
                else {
                    fileI->seekg(-6, std::ios::cur);
                }
            }

            if (character == '<') {
                fileI->read(buffChar, 2);
                if (strncmp(buffChar, "E>", 2) == 0) {
                    fileO.write(br, 2);
                    continue;
                }
                else {
                    fileI->seekg(-2, std::ios::cur);
                }
            }

            if (character == 0x0D) {
                fileI->get(buffChar[0]);
                if (buffChar[0] == 0x0A) {
                    continue;
                }
                else {
                    fileI->seekg(-1, std::ios::cur);
                }
            }

            character += decodeMoves[decodeIndex++];
            if (decodeIndex >= decodeMovesSize) {
                decodeIndex = 0;
            }

            fileO.write(&character, 1);
        }
    }

    free(buffChar);
    fileO.close();
}

void codeFile(std::fstream* fileI, char* filePath) {

    std::fstream fileO;
    fileO.open((std::string(filePath) + ".cnv").c_str(), std::ios::out | std::ios::binary);

    char character = 0;
    char* buffChar = (char*)malloc(6 * sizeof(char));
    char br[] = { 0x0D, 0x0A };
    unsigned int brCounter = 0;

    while (!fileI->eof()) {
        while (fileI->get(character)) {

            if (character == '{') {
                fileI->read(buffChar, 6);
                if (strncmp(buffChar, "<C:6>}", 6) == 0) {
                    fileO.write("{<C:6>}", 7 * sizeof(char));
                    fileO.write(br, 2 * sizeof(char));
                    fileI->seekg(2, std::ios::cur);
                    continue;
                }
                else {
                    fileI->seekg(-6, std::ios::cur);
                }
            }

            if (character == 0x0D) {
                fileI->get(buffChar[0]);
                if (buffChar[0] == 0x0A) {
                    fileO.write("<E>", 3);
                    if (++brCounter % 6 == 0) {
                        fileO.write(br, 2 * sizeof(char));
                    }
                    continue;
                }
                else {
                    fileI->seekg(-1, std::ios::cur);
                }
            }

            character -= decodeMoves[decodeIndex++];
            if (decodeIndex >= decodeMovesSize) {
                decodeIndex = 0;
            }

            fileO.write(&character, sizeof(char));
        }
    }

    free(buffChar);
    fileO.close();
}

int main(int args, char** argv)
{
    if (args < 3) {
        std::cout << "Usage: CnvRexCode *path_to_file.cnv* *option*" << std::endl;
        std::cout << "Options: D - decode / C - code" << std::endl;
        std::cout << "example: CnvRexCode \"C:\\RexAdventures\\data\\application.def\" D\n";
        return 0;
    }

    std::fstream fileI;
    fileI.open(argv[1], std::ios::in | std::ios::binary);
    if (fileI.good() == false) {
        std::cout << "Unable to open file: " << argv[1] << std::endl;
        return 1;
    }

    char* option = argv[2];
    if (strncmp(option, "D", 1) == 0) {
        decodeFile(&fileI, argv[1]);
    }
    else if (strncmp(option, "C", 1) == 0) {
        codeFile(&fileI, argv[1]);
    }
    else {
        std::cout << "Wrong option." << std::endl;
    }

    fileI.close();
    std::cout << "Done." << std::endl;
    return 0;
}