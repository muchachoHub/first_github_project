<?php

/**
 * %file
 *  Contains functions necessary for calling main Moodle web services.
 */

/**
 * Initialize a Moodle class.
 *
 * @param $moodle
 *   A moodle class.
 * @param $host
 *   A host instance.
 * @param $dir
 *   A Moodle directory for current host instance.
 * @return Moodle class
 *   An initialized Moodle class.
 */
function _init_moodle($moodle, $host, $dir) {
  $token = 'cba595d7437bc73fb64546822dd23303';
  //$host = $_SERVER['HTTP_HOST'];
  $server = 'http://' . $host;
  //$dir = '/moodle'; // May be null if moodle runs in the root directory in the server.
  // Initialize the class.
  $fields = array('token' => $token, 'server' => $server, 'dir' => $dir);
  return $moodle->init($fields);
}

/**
 * Get a Moodle user account by its idnumber.
 *
 * @param $idnumber
 *   A user id.
 * @param $moodle
 *   A moodle class.
 * @return array
 *   An array of user properties.
 */
function _get_moodle_user($idnumber, $moodle) {
  // Init a Moodle class
  //$moodle = new Moodle();
  //_init_moodle($moodle);

  // Usage examples.
  // Normally, a function returns something useful, such as an array of user properties, or TRUE, on success.
  // When something happens the return is FALSE, and the last error string is in $moodle->error string.
  // A lot of things need to be done in Moodle for web services API to work properly. See readme in this project.

  $user = $moodle->getUser($idnumber);
  if ($user) {
    // Success, normal result.
    //dpm($user);
    return $user;
  } else {
    // Error
    //drupal_set_message(t("An error has occured while retrieving a Moodle account with idnumber %id.", array('%id' => $idnumber)), 'error');
    //watchdog('Helper Moodle', "Error retrieving Moodle account with idnumber %id. Error: " . print_r($moodle->error, TRUE), array('%id' => $idnumber), WATCHDOG_ERROR);
    //dpm($moodle->error);
    return null;
  }
  return null;
}

/**
 * Create a Moodle user account.
 *
 * @param $account
 *   A user account.
 * @param $host
 *   A host instance.
 * @param $dir
 *   A Moodle directory for current host instance.
 * @return void
 */
function _create_moodle_user($account, $host, $dir) {
  // Init a Moodle class
  $moodle = new Moodle();
  _init_moodle($moodle,$host,$dir);

  // Usage examples.
  // Normally, a function returns something useful, such as an array of user properties, or TRUE, on success.
  // When something happens the return is FALSE, and the last error string is in $moodle->error string.
  // A lot of things need to be done in Moodle for web services API to work properly. See readme in this project.
  $fields = _set_profile_fields($account);
  $user = $moodle->createUser($fields);
  if ($user) {
    // Normal result contains a 2-member array with new user id and username.
    //dpm($user);
   //drupal_set_message(t("The Moodle account for user '%user' has been created.", array('%user' => $account->name)), 'status'); 
  } else {
    // Error
    drupal_set_message(t("An error has occured while creating a Moodle account for user '%user'.", array('%user' => $account->name)), 'error');
    watchdog('Helper Moodle', "Error creating Moodle account for user '%user'. Error: " . print_r($moodle->error, TRUE), array('%user' => $account->name), WATCHDOG_ERROR);
    //dpm($moodle->error);
  }
}

/**
 * Update a Moodle user account.
 *
 * @param $account
 *   A user account.
 * @param $host
 *   A host instance.
 * @param $dir
 *   A Moodle directory for current host instance.
 * @return void 
 */
function _update_moodle_user($account, $host, $dir) {
  // Init a Moodle class
  $moodle = new Moodle();
  _init_moodle($moodle,$host,$dir);
  
  $moodle_user = _get_moodle_user($account->uid, $moodle);

  if ($moodle_user) {
	$fields = _set_profile_fields($account);
	$result = $moodle->updateUser($fields);
	if ($result) {
		//dpm($result);        // Normal result.
		//drupal_set_message(t("The Moodle account for user '%user' has been updated.", array('%user' => $account->name)), 'status'); 
	} else {
		// Error
		drupal_set_message(t("An error has occured while updating the Moodle account for user '%user'.", array('%user' => $account->name)), 'error');
		watchdog('Helper Moodle', "Error updating Moodle account for user '%user'. Error: " . print_r($moodle->error, TRUE), array('%user' => $account->name), WATCHDOG_ERROR);
		//dpm($moodle->error);
	}
  }
}

/**
 * Delete a Moodle user account.
 *
 * @param $account
 *   A user account.
 * @param $host
 *   A host instance.
 * @param $dir
 *   A Moodle directory for current host instance.
 * @return void
 */
function _delete_moodle_user($account, $host, $dir) {
  // Init a Moodle class
  $moodle = new Moodle();
  _init_moodle($moodle, $host, $dir);

  $moodle_user = _get_moodle_user($account->uid, $moodle);

  if ($moodle_user) {
	  $deleted = $moodle->deleteUser($account->uid);
	  if ($deleted) {
		//dpm($deleted);        // Normal result.
		//drupal_set_message(t("The Moodle account with idnumber %id has been deleted.", array('%id' => $account->uid)), 'status'); 
	  } else {
		// Error
		drupal_set_message(t("An error has occured while deleting the Moodle account with idnumber %id.", array('%id' => $account->uid)), 'error');
		watchdog('Helper Moodle', "Error deleting Moodle account with idnumber %id. Error: " . print_r($moodle->error, TRUE), array('%id' => $account->uid), WATCHDOG_ERROR);
		//dpm($moodle->error);
	  }
  }
}

/**
 * Enroll a user as a participant in a course.
 *
 * @param $account
 *   A user account.
 * @param $course_id
 *   A course id. 
 * @return void
 */
function _enroll_participant_to_course($account, $idnumber) {
  // Init a Moodle class
  $moodle = new Moodle();
  _init_moodle($moodle);

  // get course idnumber (Need to rewrite this as a web service to get rid of module Moodle Connector.)
  // Connect to the Moodle database.
  $con = moodle_connector_connect();
  if (!$con) {
    watchdog('Commerce Moodle', "Error connecting to the Moodle database.");
    return FALSE;
  }

  try {
    // Get Moodle course id, from course number.
    $course_id = $con->query("SELECT id FROM {course} WHERE idnumber = :idnumber", array(
          ':idnumber' => $idnumber,
        ))->fetchField();

    if (!$course_id) {
      watchdog('Commerce Moodle', "Cannot enroll user because a course with the internal number '%course' does not exist in Moodle.", array(
        '%course' => $internal_number,
      ));
      return FALSE;
    }
  } catch (Exception $e) {
    watchdog('commerce_moodle', 'Error getting data from the Moodle database: ' . print_r($e->getMessage(), TRUE), array(), WATCHDOG_ERROR);
    return FALSE;
  }

  $enrolled = $moodle->enrollUser($account->uid, $course_id);
  if ($enrolled) {
    //dpm($enrolled);      // Success, normal result.
    //drupal_set_message(t("The user '%user' has been enrolled to course id %cid.", array('%user' => $account->name,'%cid' => $course_id)), 'status'); 
  } else {
    // Error
    drupal_set_message(t("An error has occured while enrolling '%user' to course id %cid.", array('%user' => $account->name, '%cid' => $course_id)), 'error');
    watchdog('Helper Moodle', "Error enrolling Moodle account for user '%user' to course id %cid. Error: " . print_r($moodle->error, TRUE), array('%user' => $account->name, '%cid' => $course_id), WATCHDOG_ERROR);
    //dpm($moodle->error);
  }
}

/**
 * Enroll a user as a group member.
 *
 * @param $account
 *   A user account.
 * @param $group_id
 *   A group id. 
 * @return void
 */
function _enroll_participant_to_group($account, $group_id) {
  // Init a Moodle class
  $moodle = new Moodle();
  _init_moodle($moodle);

  $enrolled = $moodle->enrollGroupMember($account->uid, $group_id);

  if ($enrolled) {
    //dpm($enrolled);      // Success, normal result.
    //drupal_set_message(t("The user '%user' has been enrolled to group id %gid.", array('%user' => $account->name,'%gid' => $group_id)), 'status'); 
  } else {
    // Error
    drupal_set_message(t("An error has occured while enrolling user '%user' to group id %gid.", array('%user' => $account->name, '%gid' => $group_id)), 'error');
    watchdog('Helper Moodle', "Error enrolling user '%user' to group id %gid. Error: " . print_r($moodle->error, TRUE), array('%user' => $account->name, '%gid' => $group_id), WATCHDOG_ERROR);
    //dpm($moodle->error);
  }
}

/**
 * Returns all groups in specified course.
 *
 * @param $idnumber
 *   A course id number.
 * @return array
 *   An array of course groups.
 */
function _get_course_groups($idnumber) {
  // Init a Moodle class
  $moodle = new Moodle();
  _init_moodle($moodle);

  // get course idnumber (Need to rewrite this as a web service to get rid of module Moodle Connector.)
  // Connect to the Moodle database.
  $con = moodle_connector_connect();
  
  if (!$con) {
    watchdog('Commerce Moodle', "Error connecting to the Moodle database.");
    return FALSE;
  }

  try {
    // Get Moodle course id, from course number.
    $course_id = $con->query("SELECT id FROM {course} WHERE idnumber = :idnumber", array(
          ':idnumber' => $idnumber,
        ))->fetchField();

    if (!$course_id) {
      watchdog('Commerce Moodle', "Cannot retrieve course groups because a course with the internal number '%course' does not exist in Moodle.", array(
        '%course' => $internal_number,
      ));
      return FALSE;
    }
  } catch (Exception $e) {
    watchdog('commerce_moodle', 'Error getting data from the Moodle database: ' . print_r($e->getMessage(), TRUE), array(), WATCHDOG_ERROR);
    return FALSE;
  }

  // we do not use the webservice _get_course_groups as idnumber is not included in group web service functions.
  // we need this idnumber to display groups grouped by module
  // https://tracker.moodle.org/i#browse/MDL-43949
  //$groups = $moodle->getCourseGroups($course_id);

  /* if ($groups) {
    return $groups;
    } else {
    // Error
    drupal_set_message(t("An error has occured while retrieving course groups for course id %cid.", array('%cid' => $course_id)), 'error');
    watchdog('Helper Moodle', "Error while retrieving course groups for course id %cid. Error: ". print_r($moodle->error, TRUE), array('%cid' => $course_id), WATCHDOG_ERROR);
    //dpm($moodle->error);
    return FALSE;
    }
   */

  try {
    // Get Moodle course groups.
    $groups = array();
    $results = $con->query("SELECT * FROM {groups} WHERE courseid = :courseid and idnumber like '%thematic%' ORDER BY idnumber", array(
      ':courseid' => $course_id));

    foreach ($results as $group) {
      $groups[] = $group;
    }
    return $groups;
  } catch (Exception $e) {
    watchdog('commerce_moodle', 'Error getting data from the Moodle database: ' . print_r($e->getMessage(), TRUE), array(), WATCHDOG_ERROR);
    return FALSE;
  }
}

/**
 * Returns all members of a specified course.
 *
 * @param $id
 *   A group id.
 * @return array
 *   An array of group members.
 */
function _get_group_members($id) {
  // Init a Moodle class
  $moodle = new Moodle();
  _init_moodle($moodle);

  // Connect to the Moodle database.
  $con = moodle_connector_connect();
  if (!$con) {
    watchdog('Commerce Moodle', "Error connecting to the Moodle database.");
    return FALSE;
  }

  $members = $moodle->getGroupMembers($id);

  if ($members) {
    return $members;
  } else {
    // Error
    drupal_set_message(t("An error has occured while retrieving group members for group id %gid.", array('%gid' => $id)), 'error');
    watchdog('Helper Moodle', "Error while retrieving group members for group id %gid. Error: " . print_r($moodle->error, TRUE), array('%gid' => $id), WATCHDOG_ERROR);
    //dpm($moodle->error);
    return FALSE;
  }
}

/**
 * Return the required strcured values to manage the Moodle user profile fields.
 *
 * @param $account
 *   A user account.
 * @return array
 *   An array of user fields.
 */
function _set_profile_fields($account) {
  $fields = array();
  $fields['idnumber'] = $account->uid;
  $fields['username'] = strtolower($account->name);
  $fields['password'] = $account->pass;
  $fields['email'] = $account->mail;
  $fields['lang'] = $account->language;

  // Manage participant data
  if (isset($account->profile_participant)){
    $fields['firstname'] = $account->profile_participant['field_firstname']['und'][0]['value'];
    $fields['lastname'] = $account->profile_participant['field_lastname']['und'][0]['value'];
    $fields['city'] = $account->profile_participant['field_mailing_address']['und'][0]['locality'];
    $fields['country'] = $account->profile_participant['field_mailing_address']['und'][0]['country'];

    // Avatar
    $picture = file_load($account->profile_participant['field_avatar']['und'][0]['fid']);
    $avatar_path = image_style_url("thumbnail", $picture->uri);
    $avatar_path = str_replace($GLOBALS['base_url'], '', $avatar_path);
    $custom_field = array('type' => 'avatar', 'value' => $avatar_path);
    $fields['customfields'][] = $custom_field;

	// Birthdate
	$custom_field = array('type' =>'birthdate', 'value' => strtotime($account->profile_participant['field_student_birth_date']['und'][0]['value']));  
    $fields['customfields'][] = $custom_field;
	
	// Birthplace
	$custom_field = array('type' =>'birthplace', 'value' => $account->profile_participant['field_student_birth_place']['und'][0]['value']);  
    $fields['customfields'][] = $custom_field;
	
	// Gender
	// Add 1 to gender value to get related gender id in Moodle (automatically set!)
	$custom_field = array('type' =>'gender', 'value' => $account->profile_participant['field_gender']['und'][0]['value'] + 1);
    $fields['customfields'][] = $custom_field;
  }
  // Manage contributor data
  elseif (isset($account->profile_moodle_contributor)) {
    $fields['firstname'] = $account->profile_moodle_contributor['field_firstname']['und'][0]['value'];
    $fields['lastname'] = $account->profile_moodle_contributor['field_lastname']['und'][0]['value'];
    $fields['city'] = $account->profile_moodle_contributor['field_mailing_address']['und'][0]['locality'];
    $fields['country'] = $account->profile_moodle_contributor['field_mailing_address']['und'][0]['country'];

    // Avatar
    $picture = file_load($account->profile_moodle_contributor['field_avatar']['und'][0]['fid']);
    $avatar_path = image_style_url("thumbnail", $picture->uri);
    $avatar_path = str_replace($GLOBALS['base_url'], '', $avatar_path);
    $custom_field = array('type' => 'avatar', 'value' => $avatar_path);
    $fields['customfields'][] = $custom_field;

    // Signature
    if ($account->profile_moodle_contributor['field_contrib_signature']['und'][0]['fid'] != 0) {
      $picture = file_load($account->profile_moodle_contributor['field_contrib_signature']['und'][0]['fid']);
      $picture_path = file_create_url($picture->uri);
      $picture_path = str_replace($GLOBALS['base_url'], '', $picture_path);
      $custom_field = array(
        'type' => 'examiner_signature',
        'value' => $picture_path
      );
      $fields['customfields'][] = $custom_field;
    }
  }
  // Manage administrator (administrator + manager) data
  elseif (isset($account->profile_administration)){
      $fields['firstname'] = $account->profile_administration['field_firstname']['und'][0]['value'];
      $fields['lastname'] = $account->profile_administration['field_lastname']['und'][0]['value'];
      $fields['city'] = $account->profile_administration['field_mailing_address']['und'][0]['locality'];
      $fields['country'] = $account->profile_administration['field_mailing_address']['und'][0]['country'];

      // Avatar
      $picture = file_load($account->profile_administration['field_avatar']['und'][0]['fid']);
      $avatar_path = image_style_url("thumbnail", $picture->uri);
      $avatar_path = str_replace($GLOBALS['base_url'], '', $avatar_path);
      $custom_field = array('type' => 'avatar', 'value' => $avatar_path);
      $fields['customfields'][] = $custom_field;
  }

  return $fields;
}