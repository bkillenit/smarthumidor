## Synopsis

This project is an embedded system that maintains user defined humidity level in a humidor and communicates temperature and humidity levels to a web app. It uses the Arduino platform as the microcontroller and talks to a Django web application. Included are code for both pieces of software with electronic and physical schematics on the way.

##### Major project contributors are:
Blaine Killen- Idea guy and software/ electrical engineer.  
Eric Atcheson- Awesome mechanical engineer in his last year at University of Houston.

## Motivation

We love cigars. We thought that it would be cool to integrate web technology in to a humidor. Basically, we are working on this project for **fun**.

## Installation

A few steps need to be followed after a standard fork or pull to get up and running. To setup the Arduino portion of the project, you must have the Arduino base board and any of the shields that allow you to connect to the internet along with an active internet connection. You must also wire the other components as outlined in the electrical schematics file. (coming soon)

##### Arduino Setup
**Note:** You may skip steps 2 and 3 if you change the sketchbook location of your Arduino IDE to the humidor subdirectory of the repo 

1. locate the project's root folder
2. move all the files in the humidor directory to your local Arduino sketchbook folder
* Normally this is found at "~/Documents/Arduino"
3. go to the humidor folder within your Arduino sketchbook and move the DHT folder to the libraries folder within your sketchbook
* restart the Arduino IDE if it was running during this step
4. change the mac address assigned programatically assigned to the device to the mac address specified on whatever shield you are using to enable internet communication.
* this is located on line 16 of the humidor.ino file
* you might have to change the ip address assigned to the device as well if there are address conflicts. This is located on line 17 of the humidor.ino file
5. connect your arduino and upload the humidor.ino file from the Arduino IDE to the platform


##### Django Application Setup
1. go back to the project's root folder containing just the django folder
2. switch in to the django folder
* (Optional) wrap a python virtual environment around the project root using virtualenv
3. install all of the required packages for the project. ie. pip install -r "requirements.txt"
4. start the web server using django-admin.py runserver

## Contributors

Interested in contributing? Please contact the project and repository owners at:

##### Blaine Killen
- mobile: 832 577 8611
- email: blaine.j.killen@gmail.com

##### Eric Atcheson
- mobile: 281 768 0623
- email: joe2010sax@gmail.com

## License

The project is licensed under the MIT license.