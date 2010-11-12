/*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU Library General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/**
 * \file webMonitor.h
 * Web Monitoring.
 * Creates the class or managing the web output options.
 * \author Andrew Schofield
 **/

#ifndef _WEBMONITOR_H
#define _WEBMONITOR_H

#include "wx/thread.h"
#include "wx/hashmap.h"
#include "listViewClients.h"
#include "pathManager.h"
#include <vector>


#define PREF_WEBAPP_WEBAPP   wxT("WebMonitor.UseWebApp") /**< Preference setting for use of fancy web output */
#define PREF_WEBAPP_WEBAPP_DV false /**< Default value for use of fancy web output */

#define PREF_WEBAPP_SIMPLEWEB   wxT("WebMonitor.UseSimpleWeb") /**< Preference setting for use of simple web output */
#define PREF_WEBAPP_SIMPLEWEB_DV false /**< Default value for use of simple web output */

#define PREF_WEBAPP_SIMPLETEXT   wxT("WebMonitor.UseSimpleText") /**< Preference setting for use of simple text output */
#define PREF_WEBAPP_SIMPLETEXT_DV false /**< Default value for use of simple web output */

#define PREF_WEBAPP_WEBAPPLOCATION   wxT("WebMonitor.WebAppLocation") /**< Preference setting for location of fancy web output page */
#define PREF_WEBAPP_WEBAPPLOCATION_DV wxT("") /**< Default value for location of fancy web output page */

#define PREF_WEBAPP_SIMPLEWEBLOCATION   wxT("WebMonitor.SimpleWebLocation") /**< Preference setting for location of simple web output page */
#define PREF_WEBAPP_SIMPLEWEBLOCATION_DV wxT("") /**< Default value for location of simple web output page */

#define PREF_WEBAPP_SIMPLETEXTLOCATION   wxT("WebMonitor.SimpleTextLocation") /**< Preference setting for location of simple text output page */
#define PREF_WEBAPP_SIMPLETEXTLOCATION_DV wxT("") /**< Default value for location of simple text output page */

#define PREF_WEBAPP_WEBAPPTEMPLATELOCATION   wxT("WebMonitor.WebAppTemplateLocation") /**< Preference setting for location of fancy web output page */
#define PREF_WEBAPP_WEBAPPTEMPLATELOCATION_DV wxString::Format(wxT("%sfancy_template.htm"),PathManager::GetGlobalTplPath().c_str()) /**< Default value for location of fancy web output page */

#define PREF_WEBAPP_SIMPLEWEBTEMPLATELOCATION   wxT("WebMonitor.SimpleWebTemplateLocation") /**< Preference setting for location of simple web output page */
#define PREF_WEBAPP_SIMPLEWEBTEMPLATELOCATION_DV wxString::Format(wxT("%ssimple_template.htm"),PathManager::GetGlobalTplPath().c_str()) /**< Default value for location of simple web output page */

#define PREF_WEBAPP_SIMPLETEXTTEMPLATELOCATION   wxT("WebMonitor.SimpleTextTemplateLocation") /**< Preference setting for location of simple text output page */
#define PREF_WEBAPP_SIMPLETEXTTEMPLATELOCATION_DV wxString::Format(wxT("%ssimple_template.txt"),PathManager::GetGlobalTplPath().c_str()) /**< Default value for location of simple text output page */

WX_DECLARE_HASH_MAP(wxString, wxString, wxStringHash, wxStringEqual, CustomVariableHashMap); /**< CustomVariableHashMap: variable->value */

/**
 * WebMonitor class.
 * This class manages the web output options for FahMon.
 **/
class WebMonitor
{
protected:
	static wxMutex        mMutexWebMonitorAccess; /**< Lock access to this method. */
	static WebMonitor*    mInstance; /**< The single instance of the Web monitor */
	CustomVariableHashMap mCustomVariableHashMap; /** Hashmap to store any custom variables */

	/**
	 * Constructor.
	 * Contructs the WebMonitor object.
	 **/
	WebMonitor(void);

	/**
	 * Destructor.
	 **/
	~WebMonitor(void);

public:
	/**
	 * Create the instance of Web Monitor.
	 **/
	static void CreateInstance(void);

	/**
	 * Destroy the single instance of the Web Monitor.
	 * Only if it exists.
	 **/
	static void DestroyInstance(void);

	/**
	 * Retrieve the instance of Web Monitor.
	 * If an instance doesn't already exist, create it.
	 * @return An instance of class WebMonitor.
	 **/
	static WebMonitor* GetInstance(void);

	/**
	 * Write the output pages.
	 * This method is responsible for writing all 3 output format concurrently.
	 **/
	void WriteApp(void);

	/**
	 * Retrieve custom variables from custom.dat
	 * @param variable The variable name to find
	 * @return The value of the given variable
	 **/
	const wxString GetVariable(wxString const &variable);

private:
	ListViewClients   *mClientsList; /**< The client list */
	wxString         **mDataArray; /**< Array used to store client data */
	wxString           mUpdateDate; /**< The last update time for the web output */

	/**
	 * Web Template preprocessor.
	 * This scans the given string for template ids, and loads them into a vector.
	 * @param inputTemplate The string to parse
	 * @return A vector of template IDs
	 **/
	std::vector<wxString> TemplateToVector(wxString const &inputTemplate) const;

	/**
	 * Pad/truncate a string.
	 * Takes a string and pads to the right, or truncates from the right to a given length.
	 * @param text The string to pad/truncate
	 * @param length The length desired for the output string
	 * @return The padded/truncated string
	 **/
	const wxString PadToLength(wxString text, wxUint32 length);

	/**
	 * Template decoder.
	 * This method takes template code and returns the decoded string.
	 * @param templateCode The template to decode, like '\@NAME\@'
	 * @param clientId The client Id to which the decoded text applies
	 * @return The decoded template
	 **/
	const wxString DecodeTemplate(wxString templateCode, wxUint32 clientId);

	/**
	 * Template processor.
	 * This method reads in the template file, processes it, and writes out the final page.
	 * @param templateFile The location of the input template
	 * @param outputFile The location of the output file
	 **/
	void ProcessTemplate(wxString const &templateFile, wxString const &outputFile);

	/**
	 * Custom Variable processor.
	 * This method loads the custom variables from custom.dat
	 **/
	void LoadCustomVariables(void);
};

#endif /* _WEBMONITOR_H */
