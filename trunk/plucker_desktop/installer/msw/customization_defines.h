#define branding_application_suite_name       			      	= "Plucker"
#define branding_application_desktop_name     			        = "Plucker Desktop"
#define branding_application_version_number   			        = "1.8"
#define branding_application_publisher        			        = "The Plucker Team"
#define branding_application_publisher_url    			        = "www.plkr.org"
#define branding_application_support_url      			        = "www.plkr.org"
#define branding_application_updates_url      			        = "www.plkr.org"
; The conduit will fire if a .prc with this ID is on the device (and conduit is enabled)
#define branding_creator_id										= "Plkr"
#define branding_conduit_filename								= "pluckercond.dll"
; Title of conduit as will appear in the list of conduits in the HotSync manager
#define branding_conduit_title									= "Plucker"
#define branding_viewer_hires_basename                          = "viewer_hires_"
#define branding_viewer_lores_basename                          = "viewer_"
#define branding_installer_root_filename      			        = "plucker_desktop_setup"

#define branding_installer_tasks_right_click_convert 			= 1
#define branding_installer_components_internationalization		= 1
; We won't install any with the installer (the automatically_installed_channels
; is blank) we will use the optional ones in the setup wizard instead.
#define branding_installer_allow_not_setup_default_channels		= 0
#define branding_installer_include_full_plucker_manual			= 1

#define branding_installer_show_before_install_message			= 0
#define branding_installer_show_after_install_message			= 0
