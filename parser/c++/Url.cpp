/*
 * $Id: Url.cpp,v 1.8 2007/10/17 01:27:52 chrish Exp $
 *
 * Cpluck - a part of Plucker, the free off-line HTML viewer for PalmOS
 * Copyright (c) 2003 Bill Nalen (bill@nalens.com) 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#pragma warning( disable : 4786 )

#if defined(WIN32)
#include <windows.h>
#include <wininet.h>
#else
#define INTERNET_DEFAULT_HTTP_PORT 80
#endif

#include <sstream>

#include "Pattern.h"
#include "Matcher.h"
#include "GlobalFunctions.h"
#include "Url.h"

Url::Url(std::string url)
{
    m_id = 0;
    m_url = url;
    m_port = INTERNET_DEFAULT_HTTP_PORT;
    m_maxDepth = 1;
    m_bpp = 0;
    m_tables = false;
    m_homeStayOnHost = 0;

    decodeUrl("");

    buildUrl();
}

Url::Url(std::string url, Url* refUrl)
{
    m_id = 0;
    m_url = url;
    m_port = 0;
    m_bpp = 0;

    decodeUrl(refUrl->m_scheme);

    joinUrl(refUrl);

    m_maxDepth = refUrl->m_maxDepth - 1;
    m_bpp = refUrl->m_bpp;
    m_homeStayOnHost = refUrl->m_homeStayOnHost;
    m_refHostName = refUrl->m_hostName;
    m_tables = refUrl->m_tables;

    buildUrl();
}

Url::~Url()
{
}

std::string Url::getUrlString()
{
    return m_url;
}

bool Url::shouldFetch()
{
    bool shouldFetch = true;

    if (m_maxDepth == 0 && m_scheme != "mailto") {
        shouldFetch = false;
    }
    if (m_homeStayOnHost == 1) { // host
        if (m_hostName != m_refHostName) {
            shouldFetch = false;
        }
    }
    else if (m_homeStayOnHost == 2) { // domain
        if (getDomain(m_hostName) != getDomain(m_refHostName)) {
            shouldFetch = false;
        }
    }

    return shouldFetch;
}

void Url::buildUrl()
{
    std::string url = m_urlPath;

    if (m_scheme != "file") {
        if (!m_urlPath.empty() && m_urlPath[0] != '/') {
            url = '/' + url;
        }
    }
    if (!m_params.empty()) {
        url = url + ";" + m_params;
    }
    if (!m_query.empty()) {
        url = url + "?" + m_query;
    }
    if (!m_fragment.empty()) {
        url = url + "#" + m_fragment;
    }

    m_path = url;

    if (m_port != INTERNET_DEFAULT_HTTP_PORT && m_port != 0) {
        std::stringstream ss;
        ss << ":" << m_port << url;
        url = ss.str();
    }
    if (!m_hostName.empty()) {
        url = m_hostName + url;
    }
    if (!m_scheme.empty()) {
        url = m_scheme + "://" + url;
    }
    
    m_url = url;
}

bool Url::decodeUrl(std::string baseScheme)
{
    //  <scheme>://<hostname>:<port>/<path>;<params>?<query>#<fragment>

    bool allowFragments = true;

    std::string url = m_url;
    m_scheme = baseScheme;

    std::string hostName, path, params, query, fragment;

    // find scheme
    int i = url.find(':');
    if (i != std::string::npos) {
        m_scheme = toLower(url.substr(0, i));
        url = url.substr(i+1);
    }

    // find hostname
    if (url.substr(0, 2) == "//") {
        url = url.substr(2);
        if (m_scheme != "file") {
            i = url.find('/');
            if (i == std::string::npos) {
                i = url.length();
            }
            hostName = url.substr(0, i);
            url = url.substr(i);
            i = hostName.find(':');
            if (i != std::string::npos) {
                std::string port = hostName.substr(i + 1);
                m_port = atol(port.c_str());
                hostName = hostName.substr(0, i);
            }
        }
    }

    // find fragment
    if (allowFragments) {
        i = url.find_last_of('#');
        if (i != std::string::npos) {
            fragment = url.substr(i+1);
            url = url.substr(0, i);
        }
    }

    // find query
    i = url.find('?');
    if (i != std::string::npos) {
        query = url.substr(i+1);
        url = url.substr(0, i);
    }

    // find params
    i = url.find(';');
    if (i != std::string::npos) {
        params = url.substr(i+1);
        url = url.substr(0, i);
    }

    m_hostName = hostName;
    m_urlPath = url;
    m_params = params;
    m_query = query;
    m_fragment = fragment;
            
    return true;
}

void Url::joinUrl(Url* base)
{
    if (m_port == 0) {
        m_port = base->m_port;
    }
    if (m_hostName.empty()) {
        m_hostName = base->m_hostName;
    }
    
    // relative path
    if (m_urlPath.length() > 1) {
        if (m_urlPath[0] != '/' && !(m_scheme == "file" && m_urlPath[1] == ':')) {
            int at = base->m_urlPath.find_last_of('\\');
            if (at == std::string::npos) {
                at = base->m_urlPath.find_last_of('/');
                if (at == std::string::npos) {
                    at = base->m_urlPath.length();
                }
            }
            m_urlPath = base->m_urlPath.substr(0, at) + "/" + m_urlPath;
        }
    }
    else if (!m_fragment.empty()) {
        m_urlPath = base->m_urlPath;
    }
}

void Url::setStayOnHost(int how)
{
    m_homeStayOnHost = how;
    m_refHostName = m_hostName;
}

std::string Url::getDomain(std::string host)
{
    Pattern* pat;
    Matcher* mat;
    int dot;

    if (host == "localhost")
        return host;

    pat = Pattern::compile("(\\d+).(\\d+).(\\d+).(\\d+)");
    mat = pat->createMatcher("");

    mat->setString(host);
    if(mat->matches()) // dotted quad
        return host;

    dot = host.rfind('.');
    if (dot == -1)
        return "None";

    dot = host.rfind('.', dot - 1);
    if (dot == -1)
        return host;

    return host.substr(dot + 1);
}
