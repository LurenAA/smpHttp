#ifndef __CCOMMON_HPP_
#define __CCOMMON_HPP_
#include "Common.hpp"
/**
 * 包含Libuv_wrapper中的common.hpp
 * 用于存储一些公共的数据
 **/ 


/* Status Codes */
#define HTTP_STATUS_MAP(XX)                                                 \
  XX(100, CONTINUE,                        Continue)                        \
  XX(101, SWITCHING_PROTOCOLS,             Switching Protocols)             \
  XX(102, PROCESSING,                      Processing)                      \
  XX(200, OK,                              OK)                              \
  XX(201, CREATED,                         Created)                         \
  XX(202, ACCEPTED,                        Accepted)                        \
  XX(203, NON_AUTHORITATIVE_INFORMATION,   Non-Authoritative Information)   \
  XX(204, NO_CONTENT,                      No Content)                      \
  XX(205, RESET_CONTENT,                   Reset Content)                   \
  XX(206, PARTIAL_CONTENT,                 Partial Content)                 \
  XX(207, MULTI_STATUS,                    Multi-Status)                    \
  XX(208, ALREADY_REPORTED,                Already Reported)                \
  XX(226, IM_USED,                         IM Used)                         \
  XX(300, MULTIPLE_CHOICES,                Multiple Choices)                \
  XX(301, MOVED_PERMANENTLY,               Moved Permanently)               \
  XX(302, FOUND,                           Found)                           \
  XX(303, SEE_OTHER,                       See Other)                       \
  XX(304, NOT_MODIFIED,                    Not Modified)                    \
  XX(305, USE_PROXY,                       Use Proxy)                       \
  XX(307, TEMPORARY_REDIRECT,              Temporary Redirect)              \
  XX(308, PERMANENT_REDIRECT,              Permanent Redirect)              \
  XX(400, BAD_REQUEST,                     Bad Request)                     \
  XX(401, UNAUTHORIZED,                    Unauthorized)                    \
  XX(402, PAYMENT_REQUIRED,                Payment Required)                \
  XX(403, FORBIDDEN,                       Forbidden)                       \
  XX(404, NOT_FOUND,                       Not Found)                       \
  XX(405, METHOD_NOT_ALLOWED,              Method Not Allowed)              \
  XX(406, NOT_ACCEPTABLE,                  Not Acceptable)                  \
  XX(407, PROXY_AUTHENTICATION_REQUIRED,   Proxy Authentication Required)   \
  XX(408, REQUEST_TIMEOUT,                 Request Timeout)                 \
  XX(409, CONFLICT,                        Conflict)                        \
  XX(410, GONE,                            Gone)                            \
  XX(411, LENGTH_REQUIRED,                 Length Required)                 \
  XX(412, PRECONDITION_FAILED,             Precondition Failed)             \
  XX(413, PAYLOAD_TOO_LARGE,               Payload Too Large)               \
  XX(414, URI_TOO_LONG,                    URI Too Long)                    \
  XX(415, UNSUPPORTED_MEDIA_TYPE,          Unsupported Media Type)          \
  XX(416, RANGE_NOT_SATISFIABLE,           Range Not Satisfiable)           \
  XX(417, EXPECTATION_FAILED,              Expectation Failed)              \
  XX(421, MISDIRECTED_REQUEST,             Misdirected Request)             \
  XX(422, UNPROCESSABLE_ENTITY,            Unprocessable Entity)            \
  XX(423, LOCKED,                          Locked)                          \
  XX(424, FAILED_DEPENDENCY,               Failed Dependency)               \
  XX(426, UPGRADE_REQUIRED,                Upgrade Required)                \
  XX(428, PRECONDITION_REQUIRED,           Precondition Required)           \
  XX(429, TOO_MANY_REQUESTS,               Too Many Requests)               \
  XX(431, REQUEST_HEADER_FIELDS_TOO_LARGE, Request Header Fields Too Large) \
  XX(451, UNAVAILABLE_FOR_LEGAL_REASONS,   Unavailable For Legal Reasons)   \
  XX(500, INTERNAL_SERVER_ERROR,           Internal Server Error)           \
  XX(501, NOT_IMPLEMENTED,                 Not Implemented)                 \
  XX(502, BAD_GATEWAY,                     Bad Gateway)                     \
  XX(503, SERVICE_UNAVAILABLE,             Service Unavailable)             \
  XX(504, GATEWAY_TIMEOUT,                 Gateway Timeout)                 \
  XX(505, HTTP_VERSION_NOT_SUPPORTED,      HTTP Version Not Supported)      \
  XX(506, VARIANT_ALSO_NEGOTIATES,         Variant Also Negotiates)         \
  XX(507, INSUFFICIENT_STORAGE,            Insufficient Storage)            \
  XX(508, LOOP_DETECTED,                   Loop Detected)                   \
  XX(510, NOT_EXTENDED,                    Not Extended)                    \
  XX(511, NETWORK_AUTHENTICATION_REQUIRED, Network Authentication Required) \

#define MIME_INIT(XX, tar) \
  XX(acx ,"application/internet-property-stream",tar) \
  XX(ai	,"application/postscript",tar) \
  XX(aif	,"audio/x-aiff",tar) \
  XX(aifc	,"audio/x-aiff",tar) \
  XX(aiff	,"audio/x-aiff",tar) \
  XX(asf	,"video/x-ms-asf",tar) \
  XX(asr	,"video/x-ms-asf",tar) \
  XX(asx	,"video/x-ms-asf",tar) \
  XX(au	,"audio/basic",tar) \
  XX(avi	,"video/x-msvideo",tar) \
  XX(axs	,"application/olescript",tar) \
  XX(bas	,"text/plain",tar) \
  XX(bcpio	,"application/x-bcpio",tar) \
  XX(bin	,"application/octet-stream",tar) \
  XX(bmp	,"image/bmp",tar) \
  XX(c	,"text/plain",tar) \
  XX(cat	,"application/vnd.ms-pkiseccat",tar) \
  XX(cdf	,"application/x-cdf",tar) \
  XX(cer	,"application/x-x509-ca-cert",tar) \
  XX(class	,"application/octet-stream",tar) \
  XX(clp	,"application/x-msclip",tar) \
  XX(cmx	,"image/x-cmx",tar) \
  XX(cod	,"image/cis-cod",tar) \
  XX(cpio	,"application/x-cpio",tar) \
  XX(crd	,"application/x-mscardfile",tar) \
  XX(crl	,"application/pkix-crl",tar) \
  XX(crt	,"application/x-x509-ca-cert",tar) \
  XX(csh	,"application/x-csh",tar) \
  XX(css	,"text/css",tar) \
  XX(dcr	,"application/x-director",tar) \
  XX(der	,"application/x-x509-ca-cert",tar) \
  XX(dir	,"application/x-director",tar) \
  XX(dll	,"application/x-msdownload",tar) \
  XX(dms	,"application/octet-stream",tar) \
  XX(doc	,"application/msword",tar) \
  XX(dot	,"application/msword",tar) \
  XX(dvi	,"application/x-dvi",tar) \
  XX(dxr	,"application/x-director",tar) \
  XX(eps	,"application/postscript",tar) \
  XX(etx	,"text/x-setext",tar) \
  XX(evy	,"application/envoy",tar) \
  XX(exe	,"application/octet-stream",tar) \
  XX(fif	,"application/fractals",tar) \
  XX(flr	,"x-world/x-vrml",tar) \
  XX(gif	,"image/gif",tar) \
  XX(gtar	,"application/x-gtar",tar) \
  XX(gz	,"application/x-gzip",tar) \
  XX(h	,"text/plain",tar) \
  XX(hdf	,"application/x-hdf",tar) \
  XX(hlp	,"application/winhlp",tar) \
  XX(hqx	,"application/mac-binhex40",tar) \
  XX(hta	,"application/hta",tar) \
  XX(htc	,"text/x-component",tar) \
  XX(htm	,"text/html",tar) \
  XX(html	,"text/html",tar) \
  XX(htt	,"text/webviewhtml",tar) \
  XX(ico	,"image/x-icon",tar) \
  XX(ief	,"image/ief",tar) \
  XX(iii	,"application/x-iphone",tar) \
  XX(ins	,"application/x-internet-signup",tar) \
  XX(isp	,"application/x-internet-signup",tar) \
  XX(jfif	,"image/pipeg",tar) \
  XX(jpe	,"image/jpeg",tar) \
  XX(jpeg	,"image/jpeg",tar) \
  XX(jpg	,"image/jpeg",tar) \
  XX(js	,"application/x-javascript",tar) \
  XX(latex	,"application/x-latex",tar) \
  XX(lha	,"application/octet-stream",tar) \
  XX(lsf	,"video/x-la-asf",tar) \
  XX(lsx	,"video/x-la-asf",tar) \
  XX(lzh	,"application/octet-stream",tar) \
  XX(m13	,"application/x-msmediaview",tar) \
  XX(m14	,"application/x-msmediaview",tar) \
  XX(m3u	,"audio/x-mpegurl",tar) \
  XX(man	,"application/x-troff-man",tar) \
  XX(mdb	,"application/x-msaccess",tar) \
  XX(me	,"application/x-troff-me",tar) \
  XX(mht	,"message/rfc822",tar) \
  XX(mhtml	,"message/rfc822",tar) \
  XX(mid	,"audio/mid",tar) \
  XX(mny	,"application/x-msmoney",tar) \
  XX(mov	,"video/quicktime",tar) \
  XX(movie	,"video/x-sgi-movie",tar) \
  XX(mp2	,"video/mpeg",tar) \
  XX(mp3	,"audio/mpeg",tar) \
  XX(mpa	,"video/mpeg",tar) \
  XX(mpe	,"video/mpeg",tar) \
  XX(mpeg	,"video/mpeg",tar) \
  XX(mpg	,"video/mpeg",tar) \
  XX(mpp	,"application/vnd.ms-project",tar) \
  XX(mpv2	,"video/mpeg",tar) \
  XX(ms	,"application/x-troff-ms",tar) \
  XX(mvb	,"application/x-msmediaview",tar) \
  XX(nws	,"message/rfc822",tar) \
  XX(oda	,"application/oda",tar) \
  XX(p10	,"application/pkcs10",tar) \
  XX(p12	,"application/x-pkcs12",tar) \
  XX(p7b	,"application/x-pkcs7-certificates",tar) \
  XX(p7c	,"application/x-pkcs7-mime",tar) \
  XX(p7m	,"application/x-pkcs7-mime",tar) \
  XX(p7r	,"application/x-pkcs7-certreqresp",tar) \
  XX(p7s	,"application/x-pkcs7-signature",tar) \
  XX(pbm	,"image/x-portable-bitmap",tar) \
  XX(pdf	,"application/pdf",tar) \
  XX(pfx	,"application/x-pkcs12",tar) \
  XX(pgm	,"image/x-portable-graymap",tar) \
  XX(pko	,"application/ynd.ms-pkipko",tar) \
  XX(pma	,"application/x-perfmon",tar) \
  XX(pmc	,"application/x-perfmon",tar) \
  XX(pml	,"application/x-perfmon",tar) \
  XX(pmr	,"application/x-perfmon",tar) \
  XX(pmw	,"application/x-perfmon",tar) \
  XX(pnm	,"image/x-portable-anymap",tar) \
  XX(pot ,"application/vnd.ms-powerpoint",tar) \
  XX(ppm	,"image/x-portable-pixmap",tar) \
  XX(pps	,"application/vnd.ms-powerpoint",tar) \
  XX(ppt	,"application/vnd.ms-powerpoint",tar) \
  XX(prf	,"application/pics-rules",tar) \
  XX(ps	,"application/postscript",tar) \
  XX(pub	,"application/x-mspublisher",tar) \
  XX(qt	,"video/quicktime",tar) \
  XX(ra	,"audio/x-pn-realaudio",tar) \
  XX(ram	,"audio/x-pn-realaudio",tar) \
  XX(ras	,"image/x-cmu-raster",tar) \
  XX(rgb	,"image/x-rgb",tar) \
  XX(rmi	,"audio/mid",tar) \
  XX(roff	,"application/x-troff",tar) \
  XX(rtf	,"application/rtf",tar) \
  XX(rtx	,"text/richtext",tar) \
  XX(scd	,"application/x-msschedule",tar) \
  XX(sct	,"text/scriptlet",tar) \
  XX(setpay	,"application/set-payment-initiation",tar) \
  XX(setreg	,"application/set-registration-initiation",tar) \
  XX(sh	,"application/x-sh",tar) \
  XX(shar	,"application/x-shar",tar) \
  XX(sit	,"application/x-stuffit",tar) \
  XX(snd	,"audio/basic",tar) \
  XX(spc	,"application/x-pkcs7-certificates",tar) \
  XX(spl	,"application/futuresplash",tar) \
  XX(src	,"application/x-wais-source",tar) \
  XX(sst	,"application/vnd.ms-pkicertstore",tar) \
  XX(stl	,"application/vnd.ms-pkistl",tar) \
  XX(stm	,"text/html",tar) \
  XX(svg	,"image/svg+xml",tar) \
  XX(sv4cpio	,"application/x-sv4cpio",tar) \
  XX(sv4crc	,"application/x-sv4crc",tar) \
  XX(swf	,"application/x-shockwave-flash",tar) \
  XX(t	,"application/x-troff",tar) \
  XX(tar	,"application/x-tar",tar) \
  XX(tcl	,"application/x-tcl",tar) \
  XX(tex	,"application/x-tex",tar) \
  XX(texi	,"application/x-texinfo",tar) \
  XX(texinfo	,"application/x-texinfo",tar) \
  XX(tgz	,"application/x-compressed",tar) \
  XX(tif	,"image/tiff",tar) \
  XX(tiff	,"image/tiff",tar) \
  XX(tr	,"application/x-troff",tar) \
  XX(trm	,"application/x-msterminal",tar) \
  XX(tsv	,"text/tab-separated-values",tar) \
  XX(txt	,"text/plain",tar) \
  XX(ttf, "font/ttf", tar)\
  XX(uls	,"text/iuls",tar) \
  XX(ustar	,"application/x-ustar",tar) \
  XX(vcf	,"text/x-vcard",tar) \
  XX(vrml	,"x-world/x-vrml",tar) \
  XX(wav	,"audio/x-wav",tar) \
  XX(wcm	,"application/vnd.ms-works",tar) \
  XX(wdb	,"application/vnd.ms-works",tar) \
  XX(wks	,"application/vnd.ms-works",tar) \
  XX(wmf	,"application/x-msmetafile",tar) \
  XX(wps	,"application/vnd.ms-works",tar) \
  XX(wri	,"application/x-mswrite",tar) \
  XX(woff, "font/woff", tar) \
  XX(wrl	,"x-world/x-vrml",tar) \
  XX(wrz	,"x-world/x-vrml",tar) \
  XX(xaf	,"x-world/x-vrml",tar) \
  XX(xbm	,"image/x-xbitmap",tar) \
  XX(xla	,"application/vnd.ms-excel",tar) \
  XX(xlc	,"application/vnd.ms-excel",tar) \
  XX(xlm	,"application/vnd.ms-excel",tar) \
  XX(xls	,"application/vnd.ms-excel",tar) \
  XX(xlt	,"application/vnd.ms-excel",tar) \
  XX(xlw	,"application/vnd.ms-excel",tar) \
  XX(xof	,"x-world/x-vrml",tar) \
  XX(xpm	,"image/x-xpixmap",tar) \
  XX(xwd	,"image/x-xwindowdump",tar) \
  XX(z	,"application/x-compress",tar) \
  XX(zip	,"application/zip",tar) \
  
#define PTHREAD_MAX_NUM 1024
#define PTHREAD_DEFAULT_NUM 4
namespace xx
{
  class HttpResponse;
  class HttpRequest;
  class RouteWq;
  class QueueWork;
  using routeHandleFunc = void(*)(std::shared_ptr<HttpRequest>
    , std::shared_ptr<HttpResponse>,RouteWq&);
  using QueueWorkCbType = std::function<void(std::shared_ptr<QueueWork>)>;
  using QueueAfterWorkCbType = std::function<void(std::shared_ptr<QueueWork>, int status)>;
  enum TransMode {
    CHUNKED = 109,
    CHUNKED_START,
    NORMAL
  };
  enum HttpVersion{
    HTTP_09 = 9,
    HTTP_10,
    HTTP_11,
  };
  enum HttpStatus {
    #define st(nu, name, des) HTTP_##name = nu,
    HTTP_STATUS_MAP(st)
    #undef st
  };    
} // namespace xx

#endif