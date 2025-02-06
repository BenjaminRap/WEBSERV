# **PARSING OF CONFIGURATION FILE EXPLAINED**

I am gonna explain how the configuration file is parsed in our webServ.
The configuration file is parsed into a Configuration class.
The Configuration class is a map which contains ours hosts.
Each host may have multiples server.
Our host are parsed into an Host class, and our servers into a ServerConfiguration class.

What it looks like ?

__Configuration file :__
![alt text](Configuration.png)

__Configuration class :__
![alt text](ServerConfigurationExample.png)