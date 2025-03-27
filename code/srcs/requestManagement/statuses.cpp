#include <map>         // for map
#include <string>      // for basic_string
#include <utility>     // for make_pair

#include "Status.hpp"  // for Status

const std::map<int, Status>	getStatuses(void);

const std::map<int, Status>	Status::_statuses = getStatuses();

const std::map<int, Status>	getStatuses(void)
{
	std::map<int, Status>	_statuses;

	// 1xx - Informational Responses
    _statuses.insert(std::make_pair(100, Status(100, "Continue")));
    _statuses.insert(std::make_pair(101, Status(101, "Switching Protocols")));
    _statuses.insert(std::make_pair(102, Status(102, "Processing")));
    _statuses.insert(std::make_pair(103, Status(103, "Early Hints")));

    // 2xx - Successful Responses
    _statuses.insert(std::make_pair(200, Status(200, "OK")));
    _statuses.insert(std::make_pair(201, Status(201, "Created")));
    _statuses.insert(std::make_pair(202, Status(202, "Accepted")));
    _statuses.insert(std::make_pair(203, Status(203, "Non-Authoritative Information")));
    _statuses.insert(std::make_pair(204, Status(204, "No Content")));
    _statuses.insert(std::make_pair(205, Status(205, "Reset Content")));
    _statuses.insert(std::make_pair(206, Status(206, "Partial Content")));
    _statuses.insert(std::make_pair(207, Status(207, "Multi-Status")));
    _statuses.insert(std::make_pair(208, Status(208, "Already Reported")));
    _statuses.insert(std::make_pair(226, Status(226, "IM Used")));

    // 3xx - Redirection Messages
    _statuses.insert(std::make_pair(300, Status(300, "Multiple Choices")));
    _statuses.insert(std::make_pair(301, Status(301, "Moved Permanently")));
    _statuses.insert(std::make_pair(302, Status(302, "Found")));
    _statuses.insert(std::make_pair(303, Status(303, "See Other")));
    _statuses.insert(std::make_pair(304, Status(304, "Not Modified")));
    _statuses.insert(std::make_pair(307, Status(307, "Temporary Redirect")));
    _statuses.insert(std::make_pair(308, Status(308, "Permanent Redirect")));

    // 4xx - Client Errors
    _statuses.insert(std::make_pair(400, Status(400, "Bad Request")));
    _statuses.insert(std::make_pair(401, Status(401, "Unauthorized")));
    _statuses.insert(std::make_pair(402, Status(402, "Payment Required")));
    _statuses.insert(std::make_pair(403, Status(403, "Forbidden")));
    _statuses.insert(std::make_pair(404, Status(404, "Not Found")));
    _statuses.insert(std::make_pair(405, Status(405, "Method Not Allowed")));
    _statuses.insert(std::make_pair(406, Status(406, "Not Acceptable")));
    _statuses.insert(std::make_pair(407, Status(407, "Proxy Authentication Required")));
    _statuses.insert(std::make_pair(408, Status(408, "Request Timeout")));
    _statuses.insert(std::make_pair(409, Status(409, "Conflict")));
    _statuses.insert(std::make_pair(410, Status(410, "Gone")));
    _statuses.insert(std::make_pair(411, Status(411, "Length Required")));
    _statuses.insert(std::make_pair(412, Status(412, "Precondition Failed")));
    _statuses.insert(std::make_pair(413, Status(413, "Payload Too Large")));
    _statuses.insert(std::make_pair(414, Status(414, "URI Too Long")));
    _statuses.insert(std::make_pair(415, Status(415, "Unsupported Media Type")));
    _statuses.insert(std::make_pair(416, Status(416, "Range Not Satisfiable")));
    _statuses.insert(std::make_pair(417, Status(417, "Expectation Failed")));
    _statuses.insert(std::make_pair(418, Status(418, "I'm a teapot")));
    _statuses.insert(std::make_pair(421, Status(421, "Misdirected Request")));
    _statuses.insert(std::make_pair(422, Status(422, "Unprocessable Entity")));
    _statuses.insert(std::make_pair(423, Status(423, "Locked")));
    _statuses.insert(std::make_pair(424, Status(424, "Failed Dependency")));
    _statuses.insert(std::make_pair(425, Status(425, "Too Early")));
    _statuses.insert(std::make_pair(426, Status(426, "Upgrade Required")));
    _statuses.insert(std::make_pair(428, Status(428, "Precondition Required")));
    _statuses.insert(std::make_pair(429, Status(429, "Too Many Requests")));
    _statuses.insert(std::make_pair(431, Status(431, "Request Header Fields Too Large")));
    _statuses.insert(std::make_pair(451, Status(451, "Unavailable For Legal Reasons")));

    // 5xx - Server Errors
    _statuses.insert(std::make_pair(500, Status(500, "Internal Server Error")));
    _statuses.insert(std::make_pair(501, Status(501, "Not Implemented")));
    _statuses.insert(std::make_pair(502, Status(502, "Bad Gateway")));
    _statuses.insert(std::make_pair(503, Status(503, "Service Unavailable")));
    _statuses.insert(std::make_pair(504, Status(504, "Gateway Timeout")));
    _statuses.insert(std::make_pair(505, Status(505, "HTTP Version Not Supported")));
    _statuses.insert(std::make_pair(506, Status(506, "Variant Also Negotiates")));
    _statuses.insert(std::make_pair(507, Status(507, "Insufficient Storage")));
    _statuses.insert(std::make_pair(508, Status(508, "Loop Detected")));
    _statuses.insert(std::make_pair(510, Status(510, "Not Extended")));
    _statuses.insert(std::make_pair(511, Status(511, "Network Authentication Required")));
	return (_statuses);
}
