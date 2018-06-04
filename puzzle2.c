#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BOARD_ROWS 5
#define BOARD_COLS 5

#define NUM_TILES 9

const char *tiles[NUM_TILES] = {
  "123",
  "245",
  "432",
  "314",
  "213",
  "514",
  "153",
  "25",
  "54
}

struct pos {
  int r;
  int c;
}

struct place {
  int tile;
  int sense;
  int dir;
  struct pos p;
};

struct layout {
  struct place places[NUM_TILES];
}

int layout_board(struct layout lay, int board[BOARD_ROWS][BOARD_COLS]) {
  int b[BOARD_ROWS][BOARD_COLS];
  if (board == NULL) {
    board = b;
  }

  // initialize board space
  for (int r = 0; r < BOARD_ROWS; r++) {
    for (int c = 0; c < BOARD_COLS; c++) {
      board[r][c] = 0;
    }
  }

  // lay tiles out in board
  for (int i = 0; i < NUM_TILES; i++) {
    struct place cur = lay.places[i];
    int cur_len = strlen(tiles[cur.tile]);
    for (int j = 0; j < cur_len; j++) {
      // calculate current position
      int r = cur.p.r + (dir ? j : 0);
      int c = cur.p.c + (dir ? 0 : j);

      if (r < 0 || BOARD_ROWS <= r || c < 0 || BOARD_COLS <= c) {
        return 1; // tile out of bounds
      }

      if (board[r][c] != 0) {
        return 1; // tile overlap
      }

      board[r][c] = tiles[cur.tile][cur.sense ? j : cur_len - j - 1];
    }
  }

  return 0;
}

int layout_check(struct layout lay, int board[BOARD_ROWS][BOARD_COLS]) {
  int b[BOARD_ROWS][BOARD_COLS];
  if (board == NULL) {
    board = b;
  }

  if (layout_board(lay, board)) {
    return 1; // invalid board
  }

  // check symbol exclusivity
  for (int r = 0; r < BOARD_ROWS; r++) {
    int seen[256];
    for (int c = 0; c < BOARD_COLS; c++) {
      if (seen[board[r][c]]) {
        return 1; // row conflict
      }

      seen[board[r][c]] = 1;
    }
  }

  for (int c = 0; c < BOARD_COLS; c++) {
    int seen[256];
    for (int r = 0; r < BOARD_ROWS; r++) {
      if (seen[board[r][c]]) {
        return 1; // col conflict
      }

      seen[board[r][c]] = 1;
    }
  }

  return 0;
}

struct layout **layout_solve(struct layout cur) {
  if (!layout_check(cur)) {
    struct layout *sol = malloc(sizeof(struct layout));
    struct layout **ret = malloc(sizeof(struct layout*) + 1);

    *sol = cur;
    ret[0] = sol;
    ret[1] = NULL;

    return ret;
  }


}

int main(int argc, char *argv[]) {
  struct layout **solutions = layout_solve();
}
