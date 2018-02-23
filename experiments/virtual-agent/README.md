# README #

The is the README file for the `Virtual Partner` software

Niek, Jolein and Arvid

Jolein: als je wil kun je hier nog wat gebruiksaanwijzingen oid toevoegen. 

### SSH commands for HEROS computer ###
Follow these two steps first to open a `ssh` shell:
1. make sure the `ssh` agent is running and private keys are loaded:
Open `pageant` in Windows. Open `pageant` by double-clicking the computer-with-a-hat symbol in the system tray. Click `add keys` and select `niek.ppk`. Password is the same as the login for the computer (hint: ice cold...).
2. Open `putty` and open a `ssh` shell to the HEROS computer (double-click on `ssh-heroc`).

#### Removing old settings files ####
When switching on the model fit on the HEROS computer, it is possible there are still some `settings-.xml` files in the `settings` folder, immediately triggering a model fit. To remove the files, first `cd` into the settings folder:

```bash
cd  ~/repositories/bros_experiments/main-projects/matlab-vp-modelfit/settings
```

Use `ll` to list all the files in the folder.

Remove everything (including folders!!!):

```bash
rm -rf settings*
```
Remove xml files only:

```bash
rm -rf settings*.xml
```

Now you can restart the `matlabVirtualPartner.m` through the ofAppTCControl app.

#### Copying results from HEROS to the Windows PC ####

1. Open a command prompt (click on the Start menu button and start typing "command prompt")
2. Type the following command:

```bash
pscp -r niek@130.89.64.28:/home/niek/repositories/bros_experiments/
main-projects/matlab-vp-modelfit/results/
results-modelfit-230218-* C:\Users\Labuser\Desktop
```

Note that you use the correct date at the end of `results-modelfit-`. The folders with the results are now copied to the desktop.

Authentication errors? Is `pageant` running and the key `niek.ppk` added?

### Who do I talk to? ###

Niek Beckers: niekbeckers using Google's mail
You know where to find me.

[wiki]: https://bitbucket.org/ctw-bw/bros_experiments/wiki/Home
