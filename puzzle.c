#include <stdio.h>
#include <stdlib.h>

#define DEBUG 1

void print_state();
void check();

/* BOARD MANIPULATION FUNCTIONS */

/* board state */
int board[5][5] = {0, 0, 0, 0, 0};

// put on row
void pr(int r, int c, int sense, int *tile, int len) {
  for (int i = 0; i < len; i++) {
    board[r][c + i] = tile[sense ? len - i - 1 : i];
  }
}

// put on col
void pc(int r, int c, int sense, int *tile, int len) {
  for (int i = 0; i < len; i++) {
    board[r + i][c] = tile[sense ? len - i - 1 : i];
  }
}

void print_board() {
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 5; j++) {
      printf("%d", board[i][j]);
    }
    printf("\n");
  }
}

/* PERMUTATION ENGINE */

int perm[5] = {0, 1, 2, 3, 4};

void perm_reset() {
  for (int i = 0; i < 5; i++) {
    perm[i] = i;
  }
}

void perm_next() {
  for (int i = 3; i >= 0; i--) {
    if (perm[i] < perm[i + 1]) {
      for (int j = 4; j > i; j--) {
        if (perm[i] < perm[j]) {
          int temp = perm[i];
          perm[i] = perm[j];
          perm[j] = temp;

          for (int k = 0; k < (4 - i) / 2; k++) {
            int temp = perm[i + 1 + k];
            perm[i + 1 + k] = perm[4 - k];
            perm[4 - k] = temp;
          }

#if DEBUG
          printf("perm debug: %d %d %d %d %d\n",
            perm[0], perm[1], perm[2], perm[3], perm[4]);
#endif

          return;
        }
      }

      printf("PERM FAIL IMPOSSIBLE\n");
      exit(0);
    }
  }

  printf("PERM AT END WITHOUT RESET\n");
  exit(0);
}

/* 3 ROW BRUTE FORCE */

// try arrangements of 3-tiles
// in the 3x5 space starting at given row
int senses, phase;
void try_3s(int row, int *cols[5]) {
  // |||
  phase = 0;
  perm_reset();
  for (int p = 0; p < 120; p++) {
    for (senses = 0; senses < 32; senses++) {
      // five columns
      for (int i = 0; i < 5; i++) {
        pc(row, i, (senses >> i) & 1, cols[perm[i]], 3);
      }

      check();
    }

    if (p != 119) {
      perm_next();
    }
  }

  // ||=
  phase = 1;
  perm_reset();
  for (int p = 0; p < 120; p++) {
    for (senses = 0; senses < 32; senses++) {
      // two columns
      for (int i = 0; i < 2; i++) {
        pc(row, i, (senses >> i) & 1, cols[perm[i]], 3);
      }

      // three rows
      for (int i = 2; i < 5; i++) {
        pr(row + i - 2, 2, (senses >> i) & 1, cols[perm[i]], 3);
      }

      check();
    }
    
    if (p != 119) {
      perm_next();
    }
  }

  // |=|
  phase = 2;
  perm_reset();
  for (int p = 0; p < 120; p++) {
    for (senses = 0; senses < 32; senses++) {
      // one column
      pc(row, 0, senses & 1, cols[perm[0]], 3);

      // three rows
      for (int i = 1; i < 4; i++) {
        pr(row + i - 1, 1, (senses >> i) & 1, cols[perm[i]], 3);
      }
      
      // one column
      pc(row, 4, (senses >> 4) & 1, cols[perm[4]], 3);

      check();
    }
    
    if (p != 119) {
      perm_next();
    }
  }

  // =||
  phase = 3;
  perm_reset();
  for (int p = 0; p < 120; p++) {
    for (senses = 0; senses < 32; senses++) {
      // three rows
      for (int i = 0; i < 3; i++) {
        pr(row + i, 0, (senses >> i) & 1, cols[perm[i]], 3);
      }

      // two columns
      for (int i = 3; i < 5; i++) {
        pc(row, i, (senses >> i) & 1, cols[perm[i]], 3);
      }

      check();
    }
    
    if (p != 119) {
      perm_next();
    }
  }
}

/* 5 ROW BRUTE FORCE */

// try using 5-rows built of given tiles
// and fill remaining space using given 3-tiles

// 5-row state
int low, swp, t3s, t2s, b3s, b2s;
void try_5rows(int top3[3], int top2[2],
               int bot3[3], int bot2[2],
               int *cols[5]) {
  for (low = 0; low < 2; low++)     // second 5-row at row 2 or 5?
  for (swp = 0; swp < 2; swp++)     // swap second 5-row order?
  for (t3s = 0; t3s < 2; t3s++)     // forward/reverse sense of top 3-tile,
  for (t2s = 0; t2s < 2; t2s++)     // ' ' ' of top 2-tile,
  for (b3s = 0; b3s < 2; b3s++)     // ' ' ' of bottom 3-tile,
  for (b2s = 0; b2s < 2; b2s++) {   // ' ' ' of bottom 2-tile
    pr(0, 0, t3s, top3, 3);
    pr(0, 3, t2s, top2, 2);

    pr(low ? 1 : 4, swp ? 0 : 2, b3s, bot3, 3);
    pr(low ? 1 : 4, swp ? 3 : 0, b2s, bot2, 2);

    try_3s(low ? 2 : 1, cols);
  }
}

/* MAIN BODY */

/* 3-tiles */
int ABC[3] = {0, 1, 2};
int BDE[3] = {1, 3, 4};
int DCB[3] = {3, 2, 1};
int CAD[3] = {2, 0, 3};
int BAC[3] = {1, 0, 2};
int EAD[3] = {4, 0, 3};
int AEC[3] = {0, 4, 2};

/* 2-tiles */
int BE[2] = {1, 4};
int ED[2] = {4, 3};

int phase2;
int main(int argc, char *argv[]) {
  int *cols[5];

  phase2 = 0;
  printf("5-rows ABCBE BACED\n");
  cols[0] = BDE; cols[1] = DCB; cols[2] = CAD; cols[3] = EAD; cols[4] = AEC;
  try_5rows(ABC, BE, BAC, ED, cols);

  phase2 = 1;
  printf("5-rows CADBE ABCED\n");
  cols[0] = BDE; cols[1] = DCB; cols[2] = BAC; cols[3] = EAD; cols[4] = AEC;
  try_5rows(CAD, BE, ABC, ED, cols);

  phase2 = 2;
  printf("5-rows CADBE BACED\n");
  cols[0] = ABC; cols[1] = BDE; cols[2] = DCB; cols[3] = EAD; cols[4] = AEC;
  try_5rows(CAD, BE, BAC, ED, cols);
}

/* BOARD CHECKER */

void print_state() {
  printf("5-row state: low %d, swp %d, t3s %d, t2s %d, b3s %d, b2s %d, phase %d\n",
    low, swp, t3s, t2s, b3s, b2s, phase2);
  printf("3-row state: perm %d %d %d %d %d, senses %d, phase %d\n",
    perm[0], perm[1], perm[2], perm[3], perm[4], senses, phase);
}

// check board state for satisfaction
int checks = 0;
void check() {
  checks++;
  if (checks % 10000 == 0) {
    printf("%d checks\n", checks);
    print_state();
  }

#if DEBUG
  printf("checking board:\n");
  print_board();
#endif

  // check rows
  for (int i = 0; i < 5; i++) {
    int seen[5] = {0, 0, 0, 0 ,0};
    for (int j = 0; j < 5; j++) {
      if (seen[board[i][j]]) {
        return;
      }
      seen[board[i][j]] = 1;
    }
  }

  // check cols
  for (int j = 0; j < 5; j++) {
    int seen[5] = {0, 0, 0, 0 ,0};
    for (int i = 0; i < 5; i++) {
      if (seen[board[i][j]]) {
        return;
      }
      seen[board[i][j]] = 1;
    }
  }

  // print a solution
  printf("SOLUTION!\n");
  print_state();
}

