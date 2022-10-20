#include <array>
#include <cstdint>
#include <iostream>
using namespace std;

constexpr int k_number_of_cards = 52;

template<class T, unsigned m, unsigned n>
using Matrix = array<array<T,n>,n>;
using NcrLut = Matrix<uint64_t, k_number_of_cards, k_number_of_cards>;

NcrLut calculate_ncr_lut() {
    NcrLut lut;
    lut[0].fill(0);
    lut[0][0] = 1;
    for (unsigned n = 1; n <= lut.size(); ++n) {
        lut[n][0] = 1;
        for (unsigned k = 1; k <= lut[n].size(); ++k)
            lut[n][k] = lut[n-1][k] + lut[n-1][k-1];
    }
    return lut;
}

auto k_ncr = calculate_ncr_lut();

int unset_rightmost_set_bit(uint64_t& n) {
    int rmb = __builtin_ctzll(n);
    n &= ~(1ull<<rmb);
    return rmb;
    
}

uint64_t combinadic(uint64_t hand) {
    uint64_t id = 0;
    int k = 0;
    while (hand) {
        int card_k = unset_rightmost_set_bit(hand);
        id += k_ncr[card_k][++k];
    }
    return id;
}

uint64_t combinadic_7(uint64_t hand) {
    uint64_t id;
    int card1 = unset_rightmost_set_bit(hand);
    int card2 = unset_rightmost_set_bit(hand);
    int card3 = unset_rightmost_set_bit(hand);
    int card4 = unset_rightmost_set_bit(hand);
    int card5 = unset_rightmost_set_bit(hand);
    int card6 = unset_rightmost_set_bit(hand);
    int card7 = unset_rightmost_set_bit(hand);
    id = k_ncr[card1][1] + k_ncr[card2][2] + k_ncr[card3][3] + k_ncr[card4][4] +
         k_ncr[card5][5] + k_ncr[card6][6] + k_ncr[card7][7];
    return id;
}

uint64_t combinadic_5(uint64_t hand) {
    uint64_t id;
    int card1 = unset_rightmost_set_bit(hand);
    int card2 = unset_rightmost_set_bit(hand);
    int card3 = unset_rightmost_set_bit(hand);
    int card4 = unset_rightmost_set_bit(hand);
    int card5 = unset_rightmost_set_bit(hand);
    id = k_ncr[card1][1] + k_ncr[card2][2] + k_ncr[card3][3] +
         k_ncr[card4][4] + k_ncr[card5][5];
    return id;
}

int main() {
    uint64_t x = 0b1111111000000000000000000000000000000000000000000000;
    cout << x << endl;
    cout << combinadic_7(x) << endl;
    cout << sizeof(uint16_t) << endl;
}

