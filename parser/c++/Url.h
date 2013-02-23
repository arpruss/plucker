/*
 * $Id: Url.h,v 1.5 2005/05/12 03:04:23 chrish Exp $
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

#if !defined(URL_H)
#define URL_H

#include <string>

class Url  
{
public:
    Url(std::string url);
    Url(std::string url, Url* refUrl);
	virtual ~Url();

    std::string getUrlString();
    bool shouldFetch();
    void setStayOnHost(int how);

public:
    bool decodeUrl(std::string baseScheme);
    void buildUrl();
    void joinUrl(Url* base);

    std::string m_scheme;
    std::string m_params;
    std::string m_query;
    std::string m_hostName;
    std::string m_password;
    std::string m_urlPath;
    std::string m_userName;
    std::string m_fragment;
    std::string m_path;
    long m_port;

    std::string m_contentType;
    std::string m_statusCode;
    std::string m_statusText;

    int m_id;   // resolved id

    // fetching conditions
    int m_maxDepth;
    int m_bpp;
    bool m_tables;

private:
    std::string getDomain(std::string host);

    std::string m_url;
    int m_homeStayOnHost;
    std::string m_refHostName;

};

#endif // !defined(URL_H)
