package org.plkr.distiller.API;

/**
 * The interface for a status callback for <code>org.plkr.distiller.InvokePluckerBuildFromJava.invoke()</code>.
 * 
 * @author Bill Janssen
 * @version 1
 */
public interface Callback {

  /**
   * Called by plucker-build library to provide progress information to caller.
   *
   * @param number_collected number of URLs already processed
   * @param number_in_queue number of URLs still to fetch and process
   */
  void update (int number_collected, int number_in_queue);

};
