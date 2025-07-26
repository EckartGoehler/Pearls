// simple recursive sudoku solver
#include <iostream>

//#define DEBUG
class Sudoku {

public:
    Sudoku();
    void fill();
    void print() const;
    bool solve();
    bool hasOnlyOneSolution() const;

private:
    static const unsigned NUMBER_RANGE = 9;
    static const unsigned BOARD_WIDTH = 9;
    static const unsigned BOARD_SIZE = BOARD_WIDTH*BOARD_WIDTH;
    unsigned m_board[BOARD_SIZE];
    unsigned m_depth;
    unsigned getX(unsigned pos) const;
    unsigned getY(unsigned pos) const;
    unsigned int getPos(unsigned x, unsigned y) const;
    unsigned getNum(unsigned pos) const;
    bool setNum(unsigned pos, unsigned number);
    unsigned getFirstFreePos() const;
    unsigned getNextFreePos(unsigned pos) const;
    bool tryNumber(unsigned number, unsigned pos);
    bool isValid(unsigned number, unsigned pos) const;
};

// implementation
Sudoku::Sudoku()
    :  m_depth(0)
{
    for (unsigned pos = 0; pos < BOARD_SIZE; ++ pos) {
        m_board[pos] = 0;
    }
}

void Sudoku::fill()
{
    int row, col, number;
    std::cout << "Please enter row column and number (1..9, space separated), 0 to stop" << std::endl;
    while (true) {
        std::cin >> row;
        if (row <= 0) {
            break;
        }
        std::cin >> col >> number;
        if (row <= BOARD_WIDTH && col > 0 && col <= BOARD_WIDTH && number > 0 && number <= NUMBER_RANGE) {
            if (!isValid(number, getPos(col - 1, row - 1))) {
                std::cout << " Error: number=" << number << " at x=" << col << " y=" << row << " is already occupied" << std::endl;
            } else {
                unsigned pos = getPos(col-1,row-1);
                m_board[pos] = number;
                std::cout << "-> ok" << std::endl;
            }
        } else {
            std::cout << " Error: Invalid entry. Must be in [1..9]" << std::endl;
        }
    }
    std::cout << "Here the Sudoku setup: " << std::endl;
    print();
}

void Sudoku::print() const
{
    // ______________________________
    // | 1 | 2 | 3 |
    std::cout << "=====================================" << std::endl;
    for (int y = 0; y < BOARD_WIDTH; ++y) {
        std::cout << "| ";
        for (int x = 0; x < BOARD_WIDTH; ++x) {
            std::cout << m_board[getPos(x,y)];
            if (x % 3 == 2) {
                std::cout << " | ";
            } else {
                std::cout << "   ";
            }
        }
        std::cout << std::endl;
        if (y % 3 == 2) {
            std::cout << "=====================================" << std::endl;
        } else {
            std::cout << "-------------------------------------" << std::endl;
        }
    }
}

bool Sudoku::solve()
{
    unsigned pos = getFirstFreePos();
    // then try to solve this one
    unsigned number;
    for (number = 1; number <= NUMBER_RANGE && !tryNumber(number, pos); ++number) {
        // continue if not working
    }
    return number <= NUMBER_RANGE;
}

bool Sudoku::hasOnlyOneSolution() const
{
    // assuming not solved -> try to solve for each free position and check for numbers not in line with original solution
    Sudoku initialSolution = *this;
    bool solved = initialSolution.solve();
    if (!solved) {
        return false;
    }
    bool hasUniqueSolution = true;
    unsigned pos = getFirstFreePos();
    while (hasUniqueSolution) {
        if (pos >= (BOARD_SIZE)) {
            break;
        }

        for (unsigned number = 1; number <= NUMBER_RANGE; ++number) {
#ifdef DEBUG
            std::cout << "Try: x=" << getX(pos) << " y=" << getY(pos) << " number=" << number << std::endl;
#endif
            if (number != initialSolution.getNum(pos)) {
                Sudoku testSudoku = *this;
                if (testSudoku.setNum(pos, number) && testSudoku.solve()) {
                    hasUniqueSolution = false;
                }
            }
        }
        pos = getNextFreePos(pos);
    };
    return hasUniqueSolution;
}

unsigned Sudoku::getX(unsigned pos) const
{
    return pos % BOARD_WIDTH;
}
unsigned Sudoku::getY(unsigned pos) const
{
    return pos / BOARD_WIDTH;
}

unsigned int Sudoku::getPos(unsigned x, unsigned y) const 
{
    return y * BOARD_WIDTH + x;
}

unsigned Sudoku::getNum(unsigned pos) const
{
    return m_board[pos];
}

bool Sudoku::setNum(unsigned pos, unsigned number)
{
    if (isValid(number, pos)) {
        m_board[pos] = number;
        return true;
    } else {
        return false;
    }
}
unsigned Sudoku::getFirstFreePos() const {
    unsigned pos;
    for (pos = 0; pos < BOARD_SIZE && m_board[pos] != 0; ++pos) {
        // search
    }
    return pos;
}

unsigned Sudoku::getNextFreePos(unsigned pos) const
{
    ++pos;
    for (; pos < BOARD_SIZE && m_board[pos] != 0; ++pos) {
        // search
    }
    return pos;
}

// return true when succeeding to feed in the given number and all follow up numbers
bool Sudoku::tryNumber(unsigned number, unsigned pos)
{
    ++m_depth;

#ifdef DEBUG
    // indent according to depth
    for (unsigned i = 0; i < m_depth; ++i) {
        std::cout << " ";
    }
    std::cout << "[try_number] depth=" << m_depth << " x=" << getX(pos) << " y=" << getY(pos) << " number=" << number << std::endl;
#endif

    bool success = setNum(pos,number); // testwise allocate it
    if (success) {

        unsigned nextPos = getNextFreePos(pos); 

        // if not completed -> look for further numbers
        if (nextPos < BOARD_SIZE) {
            success = false;
            unsigned next_number = 0;
            for (next_number = 1; next_number <= NUMBER_RANGE && !success; ++next_number) {
                success = tryNumber(next_number, nextPos);
            }
            // no valid solution found -> revert
            if (!success) {
                m_board[pos] = 0;
            }
        } 
        // otherwise -> we are done 
    }

#ifdef DEBUG
    for (unsigned i = 0; i < m_depth; ++i) {
        std::cout << " ";
    }
    std::cout << "[try_number] depth=" << m_depth << " x=" << getX(pos) << " y=" << getY(pos) << " number=" << number << " success=" << (success ? "passed" : "failed") << std::endl;
#endif

    --m_depth;

    return success;
}

bool Sudoku::isValid(unsigned number, unsigned pos) const
{
    unsigned x0 = getX(pos), y0 = getY(pos);

    // same number in the current row elsewhere -> not valid
    for (unsigned x = 0; x < BOARD_WIDTH; ++x) {
        if (x != x0 && m_board[getPos(x,y0)] == number) {
            return false;
        }
    }

    // same number in the current column elsewhere -> not valid
    for (unsigned y = 0; y < BOARD_WIDTH; ++y) {
        if (y != y0 && m_board[getPos(x0,y)] == number) {
            return false;
        }
    }

    // same number in the current block elsewhere -> not valid
    // obtain top left position of the current block, and loop over it
    unsigned bx = (x0 / 3) * 3;
    unsigned by = (y0 / 3) * 3;
    for (unsigned x = 0; x < 3; ++x) {
        for (unsigned y = 0; y < 3; ++y) {
            if (m_board[getPos(bx + x,by + y)] == number) {
                return false;
            }
        }
    }

    return true;
}

int main()
{
    Sudoku sudoku;
    sudoku.fill();
    Sudoku initial_sudoku = sudoku;
    bool solved = sudoku.solve();
    if (solved) {
        std::cout << "Result: " << std::endl;
        sudoku.print();
        if (initial_sudoku.hasOnlyOneSolution()) {
            std::cout << "This is the only solution" << std::endl;
        } else {
            std::cout << "This is not the only solution" << std::endl;
        }
    } else {
        std::cout << "This Sudoku is not solveable" << std::endl;
    }
}