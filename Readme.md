# xbot
A linux tool for interacting with the X11 server.
It allowes you to generate mouse/keyboard events.

## Build
<pre>
 $ cd libxbot && make
 $ cd .. && make
</pre>

## Usage
<pre>
 Usage: xbot OPTION (ARGS ...)

 capture                   Capture mouse and keypress events and write
                           results to terminal (stdout).
 capture <file>            Capture mouse and keypress events and write
                           results to given file.
 run <file>                Execute all commands from given file, created
                           by capture cmd or manually. Type 'run help'
                           for file format description.

 move <Y> <X>              Move mouse pointer to relative y/x position.
 moveto <Y> <X>            Move mouse pointer to absolute y/x position.

 click                     Click left mouse button at current postion.
 click <Y> <X>             Click left mouse button at y/x postion.
 click <button>            Click mouse button: 1=Left,2=Center,3=Right,
                           4=ScrollUp,5=ScrollDown at current position.
 click <Y> <X> <button>    Click mouse button at y/x position.

 key <keys>                Send one or more key events to x11 server.
                           Type 'key help' for more information.

 mousepos                  Prints the current mouse position.
 pxcolor                   Get pixel color at current mouse position.
 pxcolor <Y> <X>           Get pixel color at given y/x position.

 wname                     Get title of currently selected window
</pre>

## Example
xbot comes with a tiny scripting language for processing a predefined
set of commands.<br>
Supported commands are:
<pre>
 move
 moveto
 click
 key
 sleep
 usleep
</pre>

Example file:
<pre>
 # This is a xbot script
 # Run it with 'xbot run <file>'
 click 100 100 1
 sleep 1
 key 'Hello World!'[return]
</pre>
