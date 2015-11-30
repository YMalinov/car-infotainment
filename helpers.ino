char Directions[][9] = {
  "N ", "NE", "E ", "SE", "S ", "SW", "W ", "NW", "N "
};

void getHeadingAsString(float heading, char **buffer) {
  if (heading == -1.0) {
    *buffer = arrow;
  } else {
    int index = (int)((heading + 23) / 45);
    *buffer = Directions[index];
  }
}
