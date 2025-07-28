// simple recursive str8ts solver
#include <iostream>

// #define DEBUG

class Str8ts {

public:
    Str8ts();
    void fill();
    void print() const;
    bool solve();
    bool hasOnlyOneSolution() const;
    float getDifficultyLevel() const;

private:
    struct Cell {
        unsigned num;
        bool blocked;
    };
    static const unsigned FREE_CELL = 0;
    static const unsigned NUMBER_RANGE = 9;
    static const unsigned BOARD_WIDTH = 9;
    static const unsigned BOARD_SIZE = BOARD_WIDTH * BOARD_WIDTH;
    Cell m_board[BOARD_SIZE];
    unsigned m_depth;
    unsigned m_tryCount;

    bool isOccupied(unsigned pos) const;
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
Str8ts::Str8ts()
    : m_depth(0)
    , m_tryCount(0)
{
    for (unsigned pos = 0; pos < BOARD_SIZE; ++pos) {
        m_board[pos].num = FREE_CELL;
        m_board[pos].blocked = false;
    }
}

void Str8ts::fill()
{
    int row, col, number;
    std::cout << "Please enter row column and number (1..9 as number, 0 or negative to block, space separated), 0 to stop" << std::endl;
    while (true) {
        std::cin >> row;
        if (row <= 0) {
            break;
        }
        bool blocked = false;
        std::cin >> col >> number;
        if (number <= 0) {
            blocked = true;
            number = -number;
        }
        if (row <= BOARD_WIDTH && col > 0 && col <= BOARD_WIDTH && (number > 0 || blocked) && number <= NUMBER_RANGE) {
            if (!blocked && !isValid(number, getPos(col - 1, row - 1))) {
                std::cout << " Error: number=" << number << " at x=" << col << " y=" << row << " is already occupied" << std::endl;
            } else {
                unsigned pos = getPos(col - 1, row - 1);
                m_board[pos].num = number;
                m_board[pos].blocked = blocked;
                std::cout << "-> ok" << std::endl;
            }
        } else {
            std::cout << " Error: Invalid entry at row=" << row << " col=" << col << " number=" << number << " Must be in [1..9]" << std::endl;
        }
    }
    std::cout << "Here the Str8ts setup: " << std::endl;
    print();
}

void Str8ts::print() const
{
    // ______________________________
    // | 1 | 2 | 3 |
    std::cout << "=====================================" << std::endl;
    for (int y = 0; y < BOARD_WIDTH; ++y) {
        std::cout << "|";
        for (int x = 0; x < BOARD_WIDTH; ++x) {
            if (m_board[getPos(x, y)].blocked) {
                std::cout << "#";
            } else {
                std::cout << " ";
            }
            if (m_board[getPos(x, y)].num == FREE_CELL) {
                if (m_board[getPos(x, y)].blocked) {
                    std::cout << "#";
                } else {
                    std::cout << "0";
                }
            } else {
                std::cout << m_board[getPos(x, y)].num;
            }

            if (m_board[getPos(x, y)].blocked) {
                std::cout << "#";
            } else {
                std::cout << " ";
            }

            if (x % 3 == 2) {
                std::cout << "|";
            } else {
                std::cout << "|";
            }
        }
        std::cout << std::endl;
        // if (y % 3 == 2) {
        //     std::cout << "=====================================" << std::endl;
        // } else {
        std::cout << "-------------------------------------" << std::endl;
        // }
    }
}

bool Str8ts::solve()
{
    m_tryCount = 0;
    unsigned pos = getFirstFreePos();
    // then try to solve this one
    unsigned number;
    for (number = 1; number <= NUMBER_RANGE && !tryNumber(number, pos); ++number) {
        // continue if not working
    }
    return number <= NUMBER_RANGE;
}

bool Str8ts::hasOnlyOneSolution() const
{
    // assuming not solved -> try to solve for each free position and check for numbers not in line with original solution
    Str8ts initialSolution = *this;
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
                Str8ts testSudoku = *this;
                if (testSudoku.setNum(pos, number) && testSudoku.solve()) {
                    hasUniqueSolution = false;
                }
            }
        }
        pos = getNextFreePos(pos);
    };
    return hasUniqueSolution;
}

float Str8ts::getDifficultyLevel() const
{
    return m_tryCount / static_cast<float>(BOARD_SIZE);
}

bool Str8ts::isOccupied(unsigned pos) const
{
    return m_board[pos].num != FREE_CELL || m_board[pos].blocked;
}

unsigned Str8ts::getX(unsigned pos) const
{
    return pos % BOARD_WIDTH;
}
unsigned Str8ts::getY(unsigned pos) const
{
    return pos / BOARD_WIDTH;
}

unsigned int Str8ts::getPos(unsigned x, unsigned y) const
{
    return y * BOARD_WIDTH + x;
}

unsigned Str8ts::getNum(unsigned pos) const
{
    return m_board[pos].num;
}

bool Str8ts::setNum(unsigned pos, unsigned number)
{
    if (isValid(number, pos)) {
        m_board[pos].num = number;
        return true;
    } else {
        return false;
    }
}
unsigned Str8ts::getFirstFreePos() const
{
    unsigned pos;
    for (pos = 0; pos < BOARD_SIZE && isOccupied(pos); ++pos) {
        // search
    }
    return pos;
}

unsigned Str8ts::getNextFreePos(unsigned pos) const
{
    ++pos;
    for (; pos < BOARD_SIZE && isOccupied(pos); ++pos) {
        // search
    }
    return pos;
}

// return true when succeeding to feed in the given number and all follow up numbers
bool Str8ts::tryNumber(unsigned number, unsigned pos)
{
    ++m_depth;
    ++m_tryCount;

#ifdef DEBUG
    // indent according to depth
    for (unsigned i = 0; i < m_depth; ++i) {
        std::cout << " ";
    }
    std::cout << "[try_number] depth=" << m_depth << " x=" << getX(pos) << " y=" << getY(pos) << " number=" << number << std::endl;
#endif

    bool success = setNum(pos, number); // testwise allocate it
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
                m_board[pos].num = FREE_CELL;
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

bool Str8ts::isValid(unsigned number, unsigned pos) const
{
    unsigned x0 = getX(pos), y0 = getY(pos);

    // same number in the current row elsewhere -> not valid
    for (unsigned x = 0; x < BOARD_WIDTH; ++x) {
        if (x != x0 && m_board[getPos(x, y0)].num == number) {
            return false;
        }
    }

    // same number in the current column elsewhere -> not valid
    for (unsigned y = 0; y < BOARD_WIDTH; ++y) {
        if (y != y0 && m_board[getPos(x0, y)].num == number) {
            return false;
        }
    }

    // str8ts conditions horizontal
    // search to the right and left for min/max numbers and number of cells
    unsigned min = number, max = number, count = 0;
    unsigned x = x0;
    while (x > 0 && !m_board[getPos(x - 1, y0)].blocked) {
        --x;
    }
    for (; x < BOARD_WIDTH && !m_board[getPos(x, y0)].blocked; ++x) {
        ++count;
        if (m_board[getPos(x, y0)].num == FREE_CELL) {
            continue;
        }
        if (x != x0 && (min == 0 || m_board[getPos(x, y0)].num < min)) {
            min = m_board[getPos(x, y0)].num;
        }
        if (x != x0 && (max == 0 || m_board[getPos(x, y0)].num > max)) {
            max = m_board[getPos(x, y0)].num;
        }
    }
    if (max - min > count - 1) {
        return false;
    }

    // str8ts conditions vertical
    // search to the right and left for min/max numbers and number of cells
    min = number, max = number, count = 0;
    unsigned y = y0;
    while (y > 0 && !m_board[getPos(x0, y - 1)].blocked) {
        --y;
    }
    for (; y < BOARD_WIDTH && !m_board[getPos(x0, y)].blocked; ++y) {
        ++count;
        if (m_board[getPos(x0, y)].num == FREE_CELL) {
            continue;
        }
        if (y != y0 && (min == 0 || m_board[getPos(x0, y)].num < min)) {
            min = m_board[getPos(x0, y)].num;
        }
        if (y != y0 && (max == 0 || m_board[getPos(x0, y)].num > max)) {
            max = m_board[getPos(x0, y)].num;
        }
    }
    if (max - min > count - 1) {
        return false;
    }

    return true;
}

int main()
{
    Str8ts str8ts;
    str8ts.fill();
    Str8ts initial_str8ts = str8ts;
    bool solved = str8ts.solve();
    if (solved) {
        std::cout << "Result: (difficulty=" << str8ts.getDifficultyLevel() << ")" << std::endl;
        str8ts.print();

        //     if (initial_str8ts.hasOnlyOneSolution()) {
        //         std::cout << "This is the only solution" << std::endl;
        //     } else {
        //         std::cout << "This is not the only solution" << std::endl;
        //     }
    } else {
        std::cout << "This Str8ts is not solveable" << std::endl;
    }
}