#include "data.h"
#include "version.h"
#include <ctype.h>
#include <gtk/gtk.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Declarations of UI objects
GtkBuilder *builder;
GtkWidget *window;
GtkWidget *buttonEnc;
GtkWidget *buttonDec;
GtkEntry *inputEnc;
GtkEntry *inputPlain;
GtkEntry *inputKey;
GtkLabel *labelStatus;
GtkEntry *entryKey;
GtkWidget *spinPasses;
GtkWidget *spinColumns;


void processKey(const char *rawKey, int numColumns, char *processedKey) {
    // Allocate memory for the processed key
    char tempKey[numColumns + 1];
    tempKey[numColumns] = '\0';

    // Initialize the processed key
    for (int i = 0; i < numColumns; i++) {
        tempKey[i] = '0'; // You can use any default value here
    }

    // Copy the raw key, convert characters to numbers, and remove repeats
    int processedIndex = 0;
    for (int i = 0; i < numColumns; i++) {
        if (isdigit(rawKey[i])) {
            int keyValue = rawKey[i] - '0';

            // Check if the key value is not already in the processed key
            int isUnique = 1;
            for (int j = 0; j < processedIndex; j++) {
                if (tempKey[j] - '0' == keyValue) {
                    isUnique = 0;
                    break;
                }
            }

            // If the key value is unique, add it to the processed key
            if (isUnique) {
                tempKey[processedIndex++] = keyValue + '0';
            }
        }
    }

    // Repeat the processed key to match the required number of columns
    while (processedIndex < numColumns) {
        for (int i = 0; i < processedIndex; i++) {
            tempKey[processedIndex++] = tempKey[i];
        }
    }

    // Copy the processed key to the output parameter
    strcpy(processedKey, tempKey);
}

// Encrypt
const gchar *encrypt(const char *message, const char *key, gint numColumns,
                     gint passes) {
  if (message == NULL || key == NULL) {
    printf("[ERROR] Encrypt or key is null!\n");
    return NULL;
  }
  int len_message = strlen(message);
  int len_key = strlen(key);
  printf("[DEBUG] len_message = %d ; len_key = %d \n", len_message, len_key);
  int columns;
  (numColumns <= 3) ? (columns = 17) : (columns = numColumns);
  int rows = (int)(len_message / columns) + 1;

   char repeatedKey[columns+1];
   processKey(key,columns,repeatedKey);


  printf("[DEBUG] repeatedKey = %s (len = %d from columns = %d)\n", repeatedKey,
         strlen(repeatedKey), columns);
  printf("[DEBUG] Creating grid with %d columns and %d rows\n", columns, rows);

  if (rows < 4) {
    printf("[WARN] the amount of rows is too small (min: 4), setting rows to "
           "4.\n");
    rows = 4;
  }
  printf("[INFO] Creating grid with %d columns and %d rows\n", columns, rows);

  // Create a 2D array for the grid
  char grid[rows][columns];

  // Fill the grid with the message
  printf("[INFO] Filling grid \n");

  int rowIndex, colIndex;

  for (rowIndex = 0; rowIndex < rows; rowIndex++) {

    for (colIndex = 0; colIndex < columns; colIndex++) {

      if (rowIndex * columns + colIndex < len_message) {

        grid[rowIndex][colIndex] = message[rowIndex * columns + colIndex];
      } else {
        grid[rowIndex][colIndex] = '\t';
      }
    }
  }

  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      if (grid[i][j] == '\t') {
        printf("● | ");
      } else {
        printf("%c | ", grid[i][j]);
      }
    }
    printf("\n");
  }

  // Permute the columns based on the key
  for (int i = 0; i < len_key; i++) {
    char temp;
    int j = key[i] - '0'; // Convert character to integer
    // Swap columns i and j
    for (int row = 0; row < rows; row++) {
      temp = grid[row][i];
      grid[row][i] = grid[row][j];
      grid[row][j] = temp;
    }
  }
  // Permute the columns based on the repeated key
  for (int i = 0; i < columns; i++) {
    char temp;
    int j = repeatedKey[i];
    // Swap columns i and j
    printf("[DEBUG] Swapping columns %d and %d\n", i, j);
    for (int row = 0; row < rows; row++) {
      temp = grid[row][i];
      grid[row][i] = grid[row][j];
      grid[row][j] = temp;
    }
  }

  // Create a new string to store the encrypted message
  char *encrypted_message = (char *)malloc(rows * columns + 1);
  int index = 0;

  // Copy the encrypted message from the grid to the new string
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      encrypted_message[index++] = grid[i][j];
    }
  }

  // Null-terminate the new string
  encrypted_message[index] = '\0';

  // Return the encrypted message
  return encrypted_message;
}

// Key function

gint keyValid(const gchar *key) {
  if (key == NULL || key[0] == '\0') {
    return -1;
  }
  for (int i = 0; i < strlen(key); i++) {
    if (isdigit(key[i]) == false) {
      return 0;
    }
  }
  return 1;
}

// Get functions
const gchar *getKey() {
  const gchar *stringKey = gtk_entry_get_text(entryKey);
  printf("[DEBUG] KeyStr:%s \n", stringKey);
  switch (keyValid(stringKey)) {
  case 1:
    return gtk_entry_get_text(GTK_ENTRY(entryKey));
    break;
  case 0:
    printf("[ERROR] Key %s is not a number!\n", stringKey);
    return NULL;
    break;
  case -1:
    printf("[ERROR] Key is empty!\n");
    return NULL;
    break;
  }
}

const gchar *getPlain() { return gtk_entry_get_text(GTK_ENTRY(inputPlain)); }
const gchar *getEnc() { return gtk_entry_get_text(GTK_ENTRY(inputEnc)); }
size_t getEncL() { return gtk_entry_get_text_length(GTK_ENTRY(inputEnc)); }
size_t getPlainL() { return gtk_entry_get_text_length(GTK_ENTRY(inputPlain)); }
gint getColumns() {
  return gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinColumns));
}
gint getPasses() {
  return gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinPasses));
}

int main(int argc, char *argv[]) {
  if (argc >= 2) {
    if (0 == strcmp(argv[1], "--version"))
      version();
  }

  gtk_init(&argc, &argv);

  builder = gtk_builder_new_from_resource(APP_PREFIX "/window_main.glade");
  window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));

  gtk_builder_connect_signals(builder, NULL);
  // g_object_unref(builder);
  // bindings
  window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
  buttonEnc = GTK_WIDGET(gtk_builder_get_object(builder, "buttonEnc"));
  buttonDec = GTK_WIDGET(gtk_builder_get_object(builder, "buttonDec"));
  inputEnc = GTK_ENTRY(gtk_builder_get_object(builder, "inputEnc"));
  inputPlain = GTK_ENTRY(gtk_builder_get_object(builder, "inputPlain"));
  labelStatus = GTK_LABEL(gtk_builder_get_object(builder, "labelStatus"));
  entryKey = GTK_ENTRY(gtk_builder_get_object(builder, "entryKey"));
  spinPasses = GTK_WIDGET(gtk_builder_get_object(builder, "spinPasses"));
  spinColumns = GTK_WIDGET(gtk_builder_get_object(builder, "spinColumns"));

  gtk_widget_show(window);
  gtk_main();
  return 0;
}

void on_buttonDec_clicked() {}

void on_buttonEnc_clicked() {

  if (getPlainL() == 0) {
    gtk_label_set_text(GTK_ENTRY(labelStatus),
                       (const gchar *)"Plain input is empty!");
  } else {

    printf("[DEBUG] Key str:%s", getKey());
    const gchar *enc_out =
        encrypt(getPlain(), getKey(), getColumns(), getPasses());

    if (enc_out != NULL) {
      gtk_entry_set_text(GTK_ENTRY(inputEnc), enc_out);
      gtk_label_set_text(GTK_ENTRY(labelStatus),
                         (const gchar *)"Message encrypted");
      free(enc_out);
    }
  }
}

void on_window_main_destroy() { gtk_main_quit(); }
