#ifndef ERROR_PAGES_HPP
# define ERROR_PAGES_HPP

//Forbidden error
# define ERROR_403_STR "<!DOCTYPE html> \
<html lang=\"fr\"> \
<head> \
    <meta charset=\"UTF-8\"> \
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> \
    <title>403</title> \
</head> \
<body style=\"font-family:sans-serif;text-align:center;padding:50px;\"> \
    <h1>403</h1> \
    <p>Forbidden</p> \
</body> \
</html>"
# define ERROR_403_INT 403


//Not found error
# define ERROR_404_STR "<!DOCTYPE html> \
<html lang=\"fr\"> \
<head> \
    <meta charset=\"UTF-8\"> \
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> \
    <title>404</title> \
</head> \
<body style=\"font-family:sans-serif;text-align:center;padding:50px;\"> \
    <h1>404</h1> \
    <p>Not Found</p> \
</body> \
</html>"
# define ERROR_404_INT 404

//Method not allowed error
# define ERROR_405_STR "<!DOCTYPE html> \
<html lang=\"fr\"> \
<head> \
    <meta charset=\"UTF-8\"> \
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> \
    <title>405</title> \
</head> \
<body style=\"font-family:sans-serif;text-align:center;padding:50px;\"> \
    <h1>405</h1> \
    <p>Method Not Allowed</p> \
</body> \
</html>"
# define ERROR_405_INT 405

//Internal servor error
# define ERROR_500_STR "<!DOCTYPE html> \
<html lang=\"fr\"> \
<head> \
    <meta charset=\"UTF-8\"> \
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> \
    <title>500</title> \
</head> \
<body style=\"font-family:sans-serif;text-align:center;padding:50px;\"> \
    <h1>500</h1> \
    <p>Internal Servor Error</p> \
</body> \
</html>"
# define ERROR_500_INT 500

//Version not supported error
# define ERROR_505_STR "<!DOCTYPE html> \
<html lang=\"fr\"> \
<head> \
    <meta charset=\"UTF-8\"> \
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> \
    <title>505</title> \
</head> \
<body style=\"font-family:sans-serif;text-align:center;padding:50px;\"> \
    <h1>505</h1> \
    <p>HTTP Version Not Supported</p> \
</body> \
</html>"
# define ERROR_505_INT 505

#endif