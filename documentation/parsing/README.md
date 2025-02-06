# **PARSING OF CONFIGURATION FILE EXPLAINED**

The configuration file is parsed into a Configuration class.<br>
The Configuration class is a map which contains our hosts.<br>
Each host may have multiples servers.<br>
Our hosts are parsed into an Host class, and our servers into a ServerConfiguration class.<br>
<br>
What it looks like ?<br>

## __Configuration file :__
![alt text](ServerConfigurationExample.png)

## __Configuration class :__
![alt text](ConfigurationClass.png)

## __Host :__
Since there is multiples host the server will be copied multiples times and stored
into the corresponding ServerConfiguration vector.
![alt text](Host_conf.png)
![alt text](Host_map.png)

An host may have an IP of different type, ipv4, ipv6 or unix_adress.
It has the corresponding constructors.
![alt text](Host_constructors.png)

## __ServerConfiguration class :__
A server has the following attributes
![alt text](ServerConfiguration_attributes.png)

__server names :__
A server may have multiples server names.
That's why we store it into a vector.
```
std::vector<std::string>	serverNames;
```
![alt text](server_names.png)

__root :__
The location of our server is stored into :
```
std::string	root;
```
![alt text](server_root.png)

__max client body size :__
The maximum size of the html body is stored like this :
```
size_t	maxClientBodySize;
```
![alt text](max_client_body_size.png)

__error pages :__
We store the errors pages into a map.
We use the error code as the key and the corresponding url as the value.
```
std::map<unsigned short, std::string>	errorPages;
```
![alt text](error_pages.png)
If there is multiples errors codes for one url, 
every errors codes has the same copied url.

If an essential error page is missing it is defined for the user.
```
# define ERROR_403_STR "/custom_403.html"
# define ERROR_403_INT 403
# define ERROR_404_STR "/custom_404.html"
# define ERROR_404_INT 404
# define ERROR_405_STR "/custom_405.html"
# define ERROR_405_INT 405
# define ERROR_500_STR "/custom_500.html"
# define ERROR_500_INT 500
```
