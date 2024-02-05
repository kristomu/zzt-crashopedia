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

// Assumes that the seek pointer is just before the board data.
std::vector<char> read_ZZT_board(std::ifstream & infile) {
	// First read two bytes to get the board size. Throw an exception
	// if the size is out of bounds. Then read the remaining bytes.
	// WARNING: This is not endian safe! I'll fix it later if needed.
	short board_length = 0;
	char * length_ptr = (char *)&board_length;
	infile.read(length_ptr, 2);

	if (!infile) {
		return {};
	}

	if (board_length < 0 || board_length > 20000) {
		throw std::runtime_error("Invalid board length!");
	}

	std::vector<char> board(2);
	std::copy(length_ptr, length_ptr + 2, board.begin());

	if (board_length == 0) {
		return board;    // all done
	}

	board.resize(board_length + 2);
	infile.read(board.data() + 2, board_length);

	if (!infile) {
		board.resize(2 + infile.gcount());
	}

	return board;
}

std::string get_board_name(std::vector<char> & board) {
	if (board.size() < 2) {
		return "";
	}

	int name_length = (unsigned char)board[2];

	std::string name(board.begin() + 3, board.begin() + 3 + name_length);

	return name;
}

int main() {
	std::string input_file = "testworld.zzt";
	std::string board_directory = "boards/";

	std::string output_file = "output.zzt";

	// Number of bytes from start till the first board.
	const int HEADER_SIZE = 512;

	std::ifstream infile(input_file.c_str());

	std::ofstream outfile(output_file.c_str());

	// Deal with the header.
	std::vector<char> header = read(infile, HEADER_SIZE);

	while (infile) {
		std::vector<char> board = read_ZZT_board(infile);
		if (!infile && board.empty()) {
			continue;
		}
		std::cout << "That board's name is\t" <<
			get_board_name(board) << std::endl;
	}
}