#!/usr/bin/env php
<?php
// CGI header obligatoire
echo "Status: 200 OK\r\n";
echo "Content-Type: text/html; charset=utf-8\r\n";
echo "\r\n"; // Fin des headers

// Corps HTML
echo "<!DOCTYPE html>\r\n";
echo "<html>\r\n";
echo "<head><title>PHP CGI Test</title></head>\r\n";
echo "<body>\r\n";
echo "<h1>Hello from PHP CGI</h1>\r\n";
echo "<p>Time: " . date("Y-m-d H:i:s") . "</p>\r\n";
echo "</body>\r\n";
echo "</html>\r\n";
?>
