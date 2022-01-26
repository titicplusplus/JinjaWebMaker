# JinjaWebMaker

JinjaWebMaker is a web server that allows you to render jinja-like templates inside your html/css/js web pages.

# Functionalities

- Autoreload when a file has changed.
- Render jinja-like templates.
- Export project to normal files.

# Installation

```bash
git clone https://github.com/titicplusplus/JinjaWebMaker.git
cd JinjaWebMaker
cmake .
make
sudo make install
```

# How to use JinjaWebMaker

## To create a new project, you need to run:

```bash
JinjaWebMaker create ExampleOfName #That will create a directory with all the files
cd ExampleOfName
```

And with your favor editor, you can open the index.jinja2.html, styles/style.jinja2.css, scripts/scripts.jinja2.js and data.json

## To start it

To start the web server, you need to run:

```bash
JinjaWebMaker server 8080 #the port is optional
```

Then, open your browser and write : [https://127.0.0.1:8080](https://127.0.0.1:8080)

## Use the powerfull templates jinja2

- See jinja2 [documentation](https://jinja2docs.readthedocs.io/en/stable/templates.html), the most part of the documentation is compitle with JinjaWebMaker.

- To create global variables, you just need to add the variables in the data.json file like that:
- To add a jinja2 file, you need to add it the 'files' part on the data.json, your file name need to have '.jinja2' before the extension's name.

```json
{
	"variables": {
		"primaryColor": "red",     <-- global variable
		"secondaryColor": "green"
	},

	"files": [
		"index.jinja2.html",
		"styles/style.jinja2.html",
		"scripts/scripts.jinja2.js",
		"new_files.jinja2.html",    <-- New files
	]
}
```

- To create a global file, you need to create a new file in the directory 'includes/', and import it :

```html
{% include "menu.html" %} #That will add the content of the file 'includes/menu.html'
```

## Export the project

To export the project, you can use the command (on the directory):

```bash
JinjaWebMaker export
```

All the .jinja2 files will be transformed and rename without the .jinja2

That's it !

# Todo list

- Improve the default files when creating the project to understand how works jinja2 for a new user.
- Change the polling to web socket.
- A better management of the user's error.

# Thanks

There are two main projects to make this project possible :
- inja, a jinja clone : [https://github.com/pantor/inja](https://github.com/pantor/inja)
- json, to parse json file and more [https://github.com/nlohmann/json](https://github.com/nlohmann/json)
