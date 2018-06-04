#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* PROBLEM SPECIFICATION */
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
  "54"
};

/* LAYOUT INFO */
struct place {
  int tile;
  int sense;
  int dir;
  int r;
  int c;
};

struct layout {
  int placed;
  struct place places[NUM_TILES];
  char used[NUM_TILES];
  char board[BOARD_ROWS][BOARD_COLS];
};

struct layout *layout_new() {
  struct layout *lay = malloc(sizeof(struct layout));

  lay->placed = 0;

  // initialize board space
  for (int r = 0; r < BOARD_ROWS; r++) {
    for (int c = 0; c < BOARD_COLS; c++) {
      lay->board[r][c] = 0;
    }
  }

  for (int i = 0; i < NUM_TILES; i++) {
    lay->used[i] = 0;
  }

  return lay;
}

// check symbol exclusivity across rows
int board_check_row(char board[BOARD_ROWS][BOARD_COLS], int r) {
  int seen[256];
  for (int c = 0; c < BOARD_COLS; c++) {
    if (seen[board[r][c]]) {
      fprintf(stderr, "row conflict %d %d %d\n", r, c, board[r][c]);
      return 1; // row conflict
    }
    seen[board[r][c]] = 1;
  }
  return 0;
}

// check symbol exclusivity down columns
int board_check_col(char board[BOARD_ROWS][BOARD_COLS], int c) {
  int seen[256];
  for (int r = 0; r < BOARD_ROWS; r++) {
    if (seen[board[r][c]]) {
      fprintf(stderr, "col conflict %d %d %d\n", r, c, board[r][c]);
      return 1; // col conflict
    }
    seen[board[r][c]] = 1;
  }
  return 0;
}

// either places the piece in the layout and returns 0
// or there's an issue and returns 1
int layout_place(struct layout *lay, struct place p) {
  int len = strlen(tiles[p.tile]);

  if (lay->used[p.tile]) {
    fprintf(stderr, "tile already used\n");
    return 1;
  }

  // check overlaps
  for (int j = 0; j < len; j++) {
    // calculate current position
    int r = p.r + (p.dir ? j : 0);
    int c = p.c + (p.dir ? 0 : j);

    if (r < 0 || BOARD_ROWS <= r || c < 0 || BOARD_COLS <= c) {
      fprintf(stderr, "tile out of bounds\n");
      return 1;
    }

    if (lay->board[r][c] != 0) {
      fprintf(stderr, "tile overlap\n");
      return 1;
    }
  }

  // temporarily place in board
  for (int j = 0; j < len; j++) {
    // calculate current position
    int r = p.r + (p.dir ? j : 0);
    int c = p.c + (p.dir ? 0 : j);

    lay->board[r][c] = tiles[p.tile][p.sense ? j : len - j - 1];
  }

  // check symbol exclusivity
  if (p.dir == 0) {
    // placed horizontally; check row and involved columns
    if (board_check_row(lay->board, p.r)) {
      goto fail;
    }
    for (int j = 0; j < len; j++) {
      if (board_check_col(lay->board, p.c + j)) {
        goto fail;
      }
    }
  } else {
    if (board_check_col(lay->board, p.c)) {
      goto fail;
    }
    for (int j = 0; j < len; j++) {
      if (board_check_row(lay->board, p.r + j)) {
        goto fail;
      }
    }
  }

  // add tile to used set
  lay->places[lay->placed++] = p;
  lay->used[p.tile] = 1;

  return 0;

fail:
  // strip out of board
  for (int j = 0; j < len; j++) {
    // calculate current position
    int r = p.r + (p.dir ? j : 0);
    int c = p.c + (p.dir ? 0 : j);

    lay->board[r][c] = 0;
  }

  return 1;
}

// leaves layout same as before called
struct layout **layout_solve(struct layout *lay) {
  int sols = 0;
  struct layout **ret = malloc(sizeof(struct layout*));
  ret[0] = NULL;

  // check finished
  if (lay->placed == NUM_TILES) {
    fprintf(stderr, "PLACED ALL TILES\n");
    struct layout *sol = malloc(sizeof(struct layout));
    ret = realloc(ret, sizeof(struct layout*) * 2);

    *sol = *lay;
    ret[0] = sol;
    ret[1] = NULL;

    return ret;
  }

  // find next open board space
  int r, c;
  int found = 0;
  for (r = 0; r < BOARD_ROWS; r++) {
    for (c = 0; c < BOARD_COLS; c++) {
      if (lay->board[r][c] == 0) {
        found = 1;
        break;
      }
    }
    if (found) {
      break;
    }
  }
  if (!found) {
    fprintf(stderr, "FAILED to find empty slot\n");
    return ret;
  }

  // try each feasible tile placement nondeterministically
  for (int i = 0; i < NUM_TILES; i++) {
    if (!lay->used[i]) {
      fprintf(stderr, "TRYING TILE %d AT R:%d C:%d\n", i, r, c);

      int sense, dir;
      for (sense = 0; sense < 2; sense++)
      for (dir = 0; dir < 2; dir++) {
        // try placing the tile
        struct place p;
        p.tile = i;
        p.sense = sense;
        p.dir = dir;
        p.r = r;
        p.c = c;

        if (!layout_place(lay, p)) {
          struct layout **res = layout_solve(lay);

          // remove from board
          int len = strlen(tiles[p.tile]);
          lay->placed--;
          lay->used[p.tile] = 0;
          for (int j = 0; j < len; j++) {
            // calculate current position
            int r = p.r + (dir ? j : 0);
            int c = p.c + (dir ? 0 : j);

            lay->board[r][c] = 0;
          }

          // aggregate results
          for (int j = 0; res[j]; j++) {
            ret = realloc(ret, sizeof(struct layout*) * (++sols + 1));
            ret[sols - 1] = res[j];
            ret[sols] = NULL;
          }

          free(res);
        }
      }
    }
  }

  return ret;
}

void layout_print(struct layout *lay) {
  for (int i = 0; i < NUM_TILES; i++) {
    struct place p = lay->places[i];
    printf("tile %d, sense %d, dir %d, row %d, col %d\n",
      p.tile, p.sense, p.dir, p.r, p.c);
  }
  printf("board:\n");
  for (int r = 0; r < BOARD_ROWS; r++) {
    for (int c = 0; c < BOARD_COLS; c++) {
      printf("%d ", lay->board[r][c]);
    }
    printf("\n");
  }
}

int main(int argc, char *argv[]) {
  int tot = 0;
  for (int i = 0; i < NUM_TILES; i++) {
    tot += strlen(tiles[i]);
  }
  if (tot != BOARD_ROWS * BOARD_COLS) {
    printf("total tile area %d does not match board size %d\n",
      tot, BOARD_ROWS * BOARD_COLS);
    exit(0);
  }

  struct layout *init = layout_new();
  struct layout **solutions = layout_solve(init);

  for (int i = 0; solutions[i]; i++) {
    layout_print(solutions[i]);
    printf("\n");
  }

  return 0;
}
