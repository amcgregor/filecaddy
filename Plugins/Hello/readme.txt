First, if you want to see what this plugin does, install plugin.prc and
run the most recent pre-release of FileCaddy.  Go to the options menu,
and enable the plugin.  You -should- see a message pop up.  You can now
disable and delete the plugin from your device, cause that's all it does.

Yes, it's a plugin that says "Hello World".  Why?  Because I had to.

To compile this example, make sure you have PRC-Tools installed properly
and have an appropriate SDK installed for the target PalmOS version,
and simply type `make veryclean all clean`.  This should (if everything
goes well) create 'plugin.prc'.

Examine the code, and start cracking!  Plugins are your friend, trust the
plugins.

Make accepts the following .phony rules:

 all - build the .prc file, leaving temporary files
 clean - remove all temporary files, leaving the .prc file alone
 veryclean - remove all temporary files, also removing the .prc file
 install - call 'pilot-xfer' to install the plugin