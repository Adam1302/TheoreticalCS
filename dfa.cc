#define DEBUG_BIT 1
#if DEBUG_BIT
#define LOG(x) std::cout << x << '\n';
#else
#define LOG(x)
#endif

#include <iostream>
#include <map>
#include <set>
#include <unordered_map>

// T: type of states
template <typename T>
class DFA {
    std::set<T> states;
    std::set<char> symbols;
    std::map<std::pair<T, char>, T> transitions;
    T initialState;
    std::set<T> finalStates;

public:
    DFA(
        std::set<T> states,
        std::set<char> symbols,
        std::map<std::pair<T, char>, T> transitions,
        T initialState,
        std::set<T> finalStates
    ) : states{states}, symbols{symbols},
    transitions{transitions}, initialState{initialState}, finalStates{finalStates} {}
};

int main() {
    std::map<std::pair<size_t, char>, size_t> transitions1 = {
        {{0, 'a'}, 2},
        {{0, 'b'}, 1},
        {{1, 'a'}, 2},
        {{1, 'b'}, 1},
        {{2, 'a'}, 2},
        {{2, 'b'}, 2}
    };


    DFA dfa1 = DFA<size_t>(
        std::set<size_t>{0, 1, 2},
        std::set<char>{'a', 'b'},
        std::map<std::pair<size_t, char>, size_t>{
            {{0, 'a'}, 2},
            {{0, 'b'}, 1},
            {{1, 'a'}, 2},
            {{1, 'b'}, 1},
            {{2, 'a'}, 2},
            {{2, 'b'}, 2},
        },
        0,
        {0, 1}
    );
    DFA dfa2 = DFA<size_t>(
        std::set<size_t>{0, 1, 2, 3},
        std::set<char>{'a', 'b'},
        std::map<std::pair<size_t, char>, size_t>{
            {{0, 'a'}, 2},
            {{0, 'b'}, 1},
            {{1, 'a'}, 0},
            {{1, 'b'}, 3},
            {{2, 'a'}, 0},
            {{2, 'b'}, 3},
            {{3, 'a'}, 1},
            {{3, 'b'}, 2}
        },
        0,
        {0, 3}
    );
}
