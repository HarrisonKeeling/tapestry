# Tapestry
A command line tool for encrypting/decrypting videos inside other videos.

The current encryption method utilizes a LSD method for proof of concept purposes.  I plan to update this program to
allow the user to specify an encryption/decryption method from a subset of standard ciphers for more practical use.

## Requirements
Tapestry is written in C and uses [FFmpeg](https://ffmpeg.org/) to encode and decode video files.

### Installation
The easiest option to install FFmpeg is through Homebrew.
```sh
brew install ffmpeg
```

However, you can install FFmpeg via their [website](https://ffmpeg.org/download.html) if you prefer.

### Compiling
You can probably use a C compiler of your choice, but I prefer [gcc](https://gcc.gnu.org/).
```sh
gcc -o tapestry main.c
```

## Usage
Currently, two operations are provided:

    -enc <cloak path> <message path>
        * Purpose: embed a video with filename `message` into a video with the filename `cloak`
        * Example: ./stego -enc cloak.mp4 message.mp4
	-dec <cloak path>
        * Purpose: attempt to pull out a video message from an encrypted video with the filename `secret.mkv`
        * Example: ./stego -dec secret.mkv

### Limitations
Since the encryption and decryption methods currently perform operations on the pixels in each videos' frame, there are
limitations regarding the dimensions of the cloak and message video files.
 * The message video file **must** be of equal or lesser pixel dimensions (in both width and height) than the cloak video.

 > You *could* encrypt a `720x720` message into a `1280×720` cloak file, but not the other way around.  The encryption
 method does not have a scaling operation yet.

### Example
Suppose you have a video called `Picklock_Tutorial.mp4` which is a DIY video containing instructions on how to picklock.
We can hide this tutorial in another video you have, say a video of the President's recent speech, named
`Presidential_Speech.mp4`.

```sh
./tapestry -enc Presidential_Speech.mp4 Picklock_Tutorial.mp4
```

You could then send `output.mkv` to a peer.

Your peer could decrypt this inconspicuous video file.

```sh
./tapestry -dec output.mkv
```

Voila!  The peer can open `message.mp4` and watch your picklocking tutorial unencrypted.