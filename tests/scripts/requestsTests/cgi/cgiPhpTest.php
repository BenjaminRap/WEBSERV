#!/usr/bin/env php

<?php
// CGI header obligatoire
echo "Status: 200 OK\r\n";
echo "Content-Type: text/html; charset=utf-8\r\n";
echo "\r\n"; // Fin des headers

// Corps HTML
echo "<!DOCTYPE html>\n";
echo "<html>\n";
echo "<head><title>PHP CGI Test</title></head>\n";
echo "<body>\n";
echo "<h1>Hello from PHP CGI</h1>\n";
echo "<p>Time: " . date("Y-m-d H:i:s") . "</p>\n";
echo "</body>\n";
echo "</html>\n";
?>
