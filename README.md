# Tapestry
A command line tool for encrypting/decrypting videos inside other videos.


![tapestry usage example](doc/media/tapestry.gif)

## Status
The current encryption method utilizes an XOR method on the message file
and the cloak file as a one-time-pad type encryption. I plan to update
this program to allow the user to specify an encryption/decryption method
from a subset of standard ciphers for more practical use.

Additionally, you'll notice some loss of quality in the encrypted and
decrypted messages when compared to the originals.  I plan to implement
and option to store encrypted bits in a 1:2 message:cloak frame format
which will allow you to preserve the entire quality of the message at
the limitation of having a cloak file with twice as many frames than
the message file.

I've begun adding noise distribution and optional cartesian distribution,
you can take a look on the [`add-noise-distribution`](https://github.com/HarrisonKeeling/tapestry/tree/add-noise-distribution)
branch.

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
        * Example: ./tapestry -enc cloak.mp4 message.mp4
	-dec <encrypted video path> <original cloak path>
        * Purpose: attempt to pull out a video message from an encrypted video with the filename `secret.mkv`
          by using the original `cloak` file as a decyprtion key
        * Example: ./tapestry -dec secret.mkv cloak.mp4


### Limitations
Since the encryption and decryption methods currently perform operations on the pixels in each videos' frame, there are
limitations regarding the dimensions of the cloak and message video files.
 * The message video file **must** be of equal or lesser pixel dimensions (in both width and height) than the cloak video.

 > You *could* encrypt a `720x720` message into a `1280×720` cloak file, but not the other way around.  The encryption
 method does not have a scaling operation yet.

### Example
Suppose you have a video called `Picklock_Tutorial.mp4` which is a DIY video containing instructions on how to picklock.
We can hide this tutorial in another video that both you and your recipient have, say a video of your favorite mountain biking trail,
named `Mountain_Biking.mp4`.

**TIP:** A cloak file with plenty of movement and colors is the best type of file to hide content in, as it makes it harder to notice slight
differences in pixel color or movement

```sh
./tapestry -enc Mountain_Biking.mp4 Picklock_Tutorial.mp4
```

You could then send `output.mkv` to a peer.

Your peer could decrypt this inconspicuous video file using the copy of the speech video they also have.

```sh
./tapestry -dec output.mkv Mountain_Biking.mp4
```

Voila!  The peer can open `output.mp4` and watch your picklocking tutorial unencrypted.