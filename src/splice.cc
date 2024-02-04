#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

// This tool takes an input ZZT file and a boards directory. It replaces
// every placeholder board in the ZZT file with the corresponding BRD file
// in the boards directory. This is used for uneditable boards, where the
// board is corrupted to provoke bugs in ZZT. Such boards naturally can't
// be generated in-place by KevEdit and must be inserted by other means.

// Placeholder boards have a board title of "PLACEHOLDER: " followed by
// the board filename.

// Currently this is just a prototype. Beware ugly code.

std::vector<char> read(std::ifstream & infile, int num_bytes) {
	std::vector<char> bytes(num_bytes);
	infile.read(bytes.data(), num_bytes);
	return bytes;
}

void write(std::ofstream & outfile, std::vector<char> bytes) {
	outfile.write(bytes.data(), bytes.size());
}

class ZZTBoard {
	public:
		// Required if it's too short for a name. The length does not
		// count itself.
		short length;

		// Required in case the board is truncated.
		unsigned char name_length;

		std::string name;
		std::vector<char> data;

		ZZTBoard() {
			length = 0;
			name_length = 0;
		}
};

// Assumes that the seek pointer is just before the board data.
ZZTBoard read_ZZT_board(std::ifstream & infile) {
	// First read two bytes to get the board size. Throw an exception
	// if the size is out of bounds. Then read the remaining bytes.
	// WARNING: This is not endian safe! I'll fix it later if needed.
	ZZTBoard board;
	infile.read((char *)&board.length, 2);

	if (board.length < 0 || board.length > 20000) {
		throw std::runtime_error("Invalid board length!");
	}

	if (board.length == 0) { return board; } // all done

	infile.read((char *)&board.name_length, 1);

	// Read the name.
	int real_length = std::min((int)board.name_length,
		(int)board.length-1);
	board.name.resize(real_length);

	infile.read((char *)board.name.data(), real_length);

	// Read everything else.
	board.data = read(infile, board.length - real_length);

	return board;
}

int main() {
	std::string input_file = "testworld.zzt";
	std::string board_directory = "boards/";

	std::string output_file = "output.zzt";

	// Number of bytes from start till the first board.
	const int HEADER_SIZE = 265;

	std::ifstream infile(input_file.c_str());

	std::ofstream outfile(output_file.c_str());

	// Deal with the header.
	std::vector<char> header = read(infile, HEADER_SIZE);

	// TODO: Lots of times, do this. Remember to check for EOF and
	// the like.
	ZZTBoard infile_board = read_ZZT_board(infile);
	ZZTBoard output_board = infile_board;
	// if name is of the right type then...
	// 		std::string pathname = board_directory + filename extracted from board name
	//		output_board = read_ZZT_board(pathname);
	// output_board.write_ZZT_board(output_board);
}