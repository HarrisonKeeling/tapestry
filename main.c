/*
 * Author:          Harrison Chase Keeling
 * Github:          github.com/HarrisonKeeling/tapestry
 *
 * File:            main.c
 * Date Created:    20/07/2018
 *
 * Purpose:
 * A command line tool for encrypting/decrypting videos inside other videos.
 * The current encryption method utilizes a LSD method for proof of concept purposes. I plan to update this program to
 * allow the user to specify an encryption/decryption method from a subset of standard ciphers for more practical use.
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>

typedef struct {
    int width;
    int height;
} video_dimension_s;

void rgb_to_binary(int n, char* binary);
unsigned int binary_to_rgb(char* binary);
video_dimension_s *video_dimensions(char* filename);

void cipher(char* cloak, char* message, void(*operation)(char*, char*), int preserve_color);
void encrypt(char* message_binary, char* cloak_binary);
void decrypt(char* message_binary, char* cloak_binary);

void print_help();

int main(int argc, char** argv) {
    if (argc < 4) {
        print_help();
        return 1;
    } else if (strcmp(argv[1], "-enc") == 0) {
        cipher(argv[2], argv[3], encrypt, 1);
    } else if (strcmp(argv[1], "-dec") == 0) {
        cipher(argv[2], argv[3], decrypt, 0);
    }
    return 0;
}

/*
 * Function:    print_help
 * Purpose:     print the usage information
 */
void print_help() {
    printf("------- Usage -------\n");
    printf("Commands:\n");

    // encrypt
    printf("\t-enc <cloak path> <message path>\n");
    printf("\t\t\t* Purpose: embed a video with filename `message` into a video with the "
           "filename `cloak`\n");
    printf("\t\t\t* Example: ./tapestry -enc cloak.mp4 message.mp4\n");

    // decrypt
    printf("\t-dec <encrypted video path> <original cloak path>\n");
    printf("\t\t\t* Purpose: attempt to pull out a video message from an encrypted video with the filename `secret.mkv` "
           "by using the original `cloak` file as a decyprtion key\n");
    printf("\t\t\t* Example: ./tapestry -dec secret.mkv cloak.mp4\n");
}

/*
 * Function:    rgb_to_binary
 * Purpose:     Convert an integer in the range [0, 255] to an 8-bit binary value
 *
 * Input:       int n:          an integer representing a value on the scale [0, 255]
 *              char* binary:   a char array of length 9 to store the 8-bit binary value in
 */
void rgb_to_binary(int n, char* binary) {
    int idx, shift, bit;

    idx = 0;
    for (shift = 7 ; shift >= 0 ; shift--)
    {
        bit = n >> shift;
        binary[idx++] = (char)((bit & 1) + '0');
    }

    binary[8] = '\0';
}

/*
 * Function:    binary_to_rgb
 * Purpose:     Convert an 8-bit binary value to an integer in the range [0, 255]
 *
 * Input:       char* binary:   a char array of length 9 to containing the 8-bit binary value to convert to an integer
 *
 * Return:      unsigned int:   an integer representing a value on the scale [0, 255]
 */
unsigned int binary_to_rgb(char* binary) {
    int rgb255 = 0, idx = 0, remainder;
    int n = atoi(binary);
    while (n != 0)
    {
        remainder = n % 10;
        n /= 10;
        rgb255 += remainder * (int)pow(2,idx);
        ++idx;
    }
    return rgb255;
}

/*
 * Function:    video_dimensions
 * Purpose:     Retrieve the width and height of a video file
 *
 * Input:       char* filename:     a char array containing the name of the file to retrieve the dimensions from
 *
 * Return:      video_dimension_s*: an struct instance containing the integer representation of the width and height
 *                                  of the video in pixels
 */
video_dimension_s *video_dimensions(char* filename) {
    video_dimension_s *dimensions;
    char *raw_dimensions;


    // Get the dimensions of the cloak file via ffmpeg's ffprobe stream command
    char* pattern = "ffprobe -v error -select_streams v:0 -show_entries stream=width,height -of csv=s=x:p=0 %s";
    char command[strlen(pattern)+strlen(filename)];
    sprintf(command, pattern, filename);
    FILE *out_stream = popen(command, "r");


    // ffprobe will return the string in the format `WIDTHxHEIGHT`, i.e. 1920x1080
    raw_dimensions = malloc(sizeof(char) * 12); //allocate storage for the dimension string
    fread(raw_dimensions, sizeof(char), 12, out_stream);

    // allocate a new video_dimension_s struct to store the width and height in
    dimensions = malloc(sizeof(video_dimension_s));

    // parse the string to pull out the individual width and height dimensions
    char *token = strtok(raw_dimensions, "x");
    dimensions->width = atoi(token);
    token = strtok(NULL, "x");
    dimensions->height = atoi(token);

    // deallocate memory
    free(raw_dimensions);

    return dimensions;
}

/*
 * Function:    encrypt
 * Purpose:     embed a video with filename `message` into a video with the filename `cloak`
 *
 * Input:       char* message_binary:   binary of a particular color value (r, g, or b) for a pixel in a frame of
 *                                      the message file
 *              char* cloak_binary:     binary of a particular color value (r, g, or b) for a pixel in a frame of
 *                                      the cloak file
 */
void encrypt(char* message_binary, char* cloak_binary) {
    for (int lsd = 4; lsd < 8; ++lsd) {
        cloak_binary[lsd] = (char) (((int) (cloak_binary[lsd] - '0') ^ (int) (message_binary[lsd - 4] - '0')) + '0');
    }
}

/*
 * Function:    decrypt
 * Purpose:     attempt to pull out a video message from an encrypted video and store the results in cloak_binary
 *
 * Input:       char* message_binary:   binary of a particular color value (r, g, or b) for a pixel in a frame of
 *                                      the message file
 *              char* cloak_binary:     binary of a particular color value (r, g, or b) for a pixel in a frame of
 *                                      the cloak file
 */
void decrypt(char* message_binary, char* cloak_binary) {
    for (int lsd = 4; lsd < 8; ++lsd) {
        cloak_binary[lsd] = (char)(((int)(cloak_binary[lsd] - '0') ^ (int)(message_binary[lsd] - '0')) + '0');
    }

    // store least significant digits from the compiled message in the most significant digits
    char temp;
    for (int lsd = 0; lsd < 4; ++lsd) {
        temp = cloak_binary[lsd];
        cloak_binary[lsd] = cloak_binary[lsd+4];
        cloak_binary[lsd+4] = temp;
    }
}

/*
 * Function:    cipher
 * Purpose:     attempt to pull out a video message from an encrypted video with the filename `secret.mkv`
 *
 * Input:       char* cloak:        char array containing the name of the file to embed into
 *              char* message       char array containing the name of the file to embed
 *              void operation:     pointer to a method that that takes in both the message_binary and cloak_binary of a
 *                                  particular color value (r, g, or b) for a pixel in a frame and performs an encryption
 *                                  or decryption mechanism on it
 *              int preserve_color: flag which determines whether to output to a format preserving the rgb values
 */
void cipher(char* cloak, char* message, void(*operation)(char*, char*), int preserve_color) {
    int x, y, cloak_values, message_values, cloak_offset, message_offset;
    char *cloak_frame, *message_frame;
    char *command;
    // create room for two 8-bit binary strings to hold an integer
    char *cloak_binary = malloc(sizeof(char) * 9);
    cloak_binary[9] = '\0';
    char *message_binary = malloc(sizeof(char) * 9);
    message_binary[9] = '\0';

    // Retrieve the dimensions of the cloak and the message you're wanting to encrypt
    video_dimension_s *cloak_dimensions = video_dimensions(cloak);
    video_dimension_s *message_dimensions = video_dimensions(message);

    // Allocate a buffer to store one cloak_frame of rgb values
    cloak_frame = malloc(
            sizeof(char) * cloak_dimensions->height * cloak_dimensions->width * 3
            );
    message_frame = malloc(
            sizeof(char) * message_dimensions->height * message_dimensions->width * 3
            );

    // open cloak and message video files in pipes so we can read in the frame data
    char* cloak_pattern = "ffmpeg -i %s -y -acodec copy tmp_audio.aac -f image2pipe "
                          "-vcodec rawvideo -pix_fmt rgb24 -color_range 2 -";
    // allocate enough space for the command pattern plus the cloak filename
    command = malloc(sizeof(char) * strlen(cloak_pattern) + strlen(cloak) + 1);
    sprintf(command, cloak_pattern, cloak);
    FILE *cloak_in = popen(command, "r");
    // free the command memory so we can use it again.
    free(command);

    char *message_pattern = "ffmpeg -i %s -f image2pipe -vcodec rawvideo -pix_fmt rgb24 -color_range 2 -";
    command = malloc(sizeof(char) * strlen(message_pattern) + strlen(message) + 1);
    sprintf(command, message_pattern, message);
    FILE *message_in = popen(command, "r");
    free(command);

    // open a write pipe, to store the composite video
    char *write_pattern;

    if (preserve_color) {
        write_pattern = "ffmpeg -y -f rawvideo -vcodec rawvideo -pix_fmt rgb24 -color_range 2 -s %ix%i -i - "
                              "-map 0:v:0 -pix_fmt bgr24 -c:v libx264rgb -preset veryslow -qp 0 tmp_compiled.mkv";
    } else {
        write_pattern = "ffmpeg -y -f rawvideo -vcodec rawvideo -pix_fmt rgb24 -color_range 2 -s %ix%i -r 25 -i - "
                        "-map 0:v:0 -vcodec mpeg4 -pix_fmt rgb24 -color_range 2 tmp_compiled.mp4";
    }

    // allocate enough space for the message plus the max size of pixels
    command = malloc(sizeof(char) * strlen(write_pattern) + 11);
    sprintf(command, write_pattern, cloak_dimensions->width, cloak_dimensions->height);
    FILE *composed_out = popen(command, "w");
    free(command);

    // process frames
    while(1)
    {
        // Read a cloak_frame from the input pipe into the buffer
        cloak_values = (int)fread(
                cloak_frame,
                sizeof(char),
                cloak_dimensions->height * cloak_dimensions->width * 3,
                cloak_in
                );

        // Read a message_frame from the input pipe into the buffer
        message_values = (int)fread(
                message_frame,
                sizeof(char),
                message_dimensions->height * message_dimensions->width * 3,
                message_in
                );

        // If the number of rgb values isn't equal to the amount in a frame, then there aren't any more frames to process
        if (cloak_values != cloak_dimensions->height*cloak_dimensions->width*3) break;

        // If there is still a frame of the message to hide, we need to hide it in the cloak.
        if (message_values == message_dimensions->height*message_dimensions->width*3) {
            // Process the frame of message to embed it
            for (y = 0; y < message_dimensions->height; ++y) {
                for (x = 0; x < message_dimensions->width; ++x) {
                    // get the location of the pixel in the cloak and the message
                    cloak_offset = (y * cloak_dimensions->width * 3) + (x * 3);
                    message_offset = (y * message_dimensions->width * 3) + (x * 3);

                    // for the red, green, and blue elements of the pixel
                    for (int color = 0; color < 3; ++color) {
                        /* get the current [0-255] ranged value in binary for the pixel location
                         * in the cloak and the message
                         */
                        rgb_to_binary(cloak_frame[cloak_offset + color], cloak_binary);
                        rgb_to_binary(message_frame[message_offset + color], message_binary);

                        operation(message_binary, cloak_binary);

                        // convert the composed binary value back to a [0-255] ranged rgb value
                        cloak_frame[cloak_offset + color] = (char)binary_to_rgb(cloak_binary);
                    }
                }
            }
        }

        // Write this cloak_frame to the output pipe
        fwrite(cloak_frame, 1, cloak_dimensions->height * cloak_dimensions->width * 3, composed_out);
    }

    // Flush and close input and output pipes
    fflush(cloak_in);
    pclose(cloak_in);
    fflush(message_in);
    pclose(message_in);
    fflush(composed_out);
    pclose(composed_out);

    //compose the intertwined video with the original cloak audio
    if (preserve_color) {
        system("ffmpeg -y -i tmp_compiled.mkv -i tmp_audio.aac -c copy -map 0:v:0 -map 1:a:0 output.mkv");
        remove("tmp_compiled.mkv");
    } else {
        system("ffmpeg -y -i tmp_compiled.mp4 -i tmp_audio.aac -c copy -map 0:v:0 -map 1:a:0 output.mp4");
        remove("tmp_compiled.mp4");
    }
    remove("tmp_audio.aac");

    // free memory
    free(cloak_binary);
    free(message_binary);
    free(cloak_dimensions);
    free(message_dimensions);
    free(cloak_frame);
    free(message_frame);
}