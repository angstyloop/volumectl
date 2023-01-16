/*
Copy the header to a source file, compile the source, and remove the temporary
source file.
a=set_volume; cp $a.h $a.c && gcc -o $a -Dtest_main $a.c -lasound && rm $a.c
*/

#include "stdio_h.h"
#include "stdlib_h.h"
#include "alsa_asoundlib_h.h"

static void error_close_exit(char* errmsg, int err, snd_mixer_t* h_mixer)
{
    if (err == 0)
        fputs(errmsg, stderr);
    else
        fprintf(stderr, errmsg, snd_strerror(err));
    if (h_mixer != NULL)
        snd_mixer_close(h_mixer);
    exit(EXIT_FAILURE);
}

void set_volume(char* device, char* selem_name, long vol) {
	long min = 0, max = 0;
    int err = 0;

    snd_mixer_t* h_mixer = NULL;
    snd_mixer_selem_id_t* sid = NULL;
    snd_mixer_elem_t* elem = NULL;

    if ((err = snd_mixer_open(&h_mixer, 1)) < 0)
        error_close_exit("Mixer open error: %s\n", err, NULL);

    if ((err = snd_mixer_attach(h_mixer, device)) < 0)
        error_close_exit("Mixer attach error: %s\n", err, h_mixer);

    if ((err = snd_mixer_selem_register(h_mixer, NULL, NULL)) < 0)
        error_close_exit("Mixer simple element register error: %s\n", err, h_mixer);

    if ((err = snd_mixer_load(h_mixer)) < 0)
        error_close_exit("Mixer load error: %s\n", err, h_mixer);

    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, selem_name);

    if ((elem = snd_mixer_find_selem(h_mixer, sid)) == NULL)
        error_close_exit("Cannot find simple element\n", 0, h_mixer);

    if ((err = snd_mixer_selem_get_playback_volume_range(elem, &min, &max)) < 0)
        error_close_exit("Mixer simple element get playback volume range error: %s\n", err, h_mixer);

    vol = vol < 0 ? 0 : vol > max ? max : vol;

	if ((err = snd_mixer_selem_set_playback_volume_all(elem, vol * max / 100)) < 0)
		error_close_exit("Volume set error: %s\n", err, h_mixer);

    snd_mixer_close(h_mixer);
}

// Compile a script and run it

#ifdef test_main

int main(int argc, char** argv)
{
    if (argc < 4) {
        printf("\n"
               "USAGE\n\t%s <DEVICE> <CONTROL> <VOLUME>\n"
               "\n"
               "INFO\n"
               "\n"
                   "\tVOLUME 0-100\n"
               "\n"
                   "\tExample values to mute the master volume:\n"
               "\n"
                   "\tDEVICE = default\n"
                   "\tCONTROL = Master\n"
                   "\tVOLUME = 0\n"
               "\n", argv[0]);
        return EXIT_FAILURE;
    }

    /***********************/
    /* Linux Control Name. */
    /***********************/
    char* selem_name = argv[2];

    /********************/
    /* Sound Card name. */
    /********************/
    char* device = argv[1];

    /******************/
    /* Volume you set */
    /******************/
    long vol = atol(argv[3]);

    set_volume(device, selem_name, vol);

    return EXIT_SUCCESS;
}

#endif // test_main
