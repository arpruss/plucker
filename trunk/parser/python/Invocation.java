package org.plkr.distiller.API;

/**
 * The interface supported by <code>org.plkr.distiller.InvokePluckerBuildFromJava</code>.
 * 
 * @author Bill Janssen
 * @version 1
 */

public interface Invocation {

    /**
     * Invokes the Plucker distiller with the specified arguments, and
     * returns any status returned from the distillation.
     *
     * @param arguments                standard command-line arguments to plucker-build
     * @param optional_output_channel  output stream to write document to, instead of a file
     * @param optional_input_string    string containing HTML to use as home document, instead of a home URL
     * @param optional_config_params   configuration parameters which override any configuration files.
     *                                 Each entry in the <code>Hashtable</code> must have a <code>String</code> key and a <code>String</code> value.
     * @param optional_status_callback if provided, the <code>update</code> method will be called periodically (typically on every fetch) to provide progress information
     * @return  an integer exit status from plucker-build; 0 indicates success, and non-zero indicates an error
     * @exception java.lang.Exception shouldn't do this, but may in the case of a bug
     */

    int invoke (java.lang.String[] arguments,
                java.io.OutputStream optional_output_channel,
                java.lang.String optional_input_String,
                java.util.Hashtable optional_config_parameters,
                org.plkr.distiller.API.Callback optional_status_callback)
      throws java.lang.Exception;
};
