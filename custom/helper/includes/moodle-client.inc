<?php

/**
 * @file
 * A class to call Moodle web services.
 * More info: http://www.rumours.co.nz/manuals/using_moodle_web_services.htm
 */
class Moodle {

  var $token = NULL;  // Token to access Moodle server. Must be configured in Moodle. See readme.
  var $server = NULL; // Moodle URL, for example http://localhost:8080.
  var $dir = NULL;    // Directory on the server. For example, /moodle. If your moodle runs as root, this is empty.
  var $error = '';    // Last error of the class. We'll write the last error here when something wrong happens.

  // The init function initializes the variable of class (so that it can be used).

  function init($fields) {
    $this->token = $fields['token'];
    $this->server = $fields['server'];
    $this->dir = $fields['dir'];
  }

  // The getUser function obtains information for a Moodle user identified by its idnumber.
  // A bug prevent to use 'core_user_get_users_by_field' if the user of the call is not an admin or
  // the user you are looking for. Therefore we set the user of the webservice as an admin in Moodle.
  // See: https://tracker.moodle.org/i#browse/MDL-42639.
  function getUser($user_id) {
    // Clear last error.
    $this->error = NULL;

    // Create XML for the request. XML must be set properly for this to work.
    $params = array('idnumber', array((string) $user_id));
    $request = xmlrpc_encode_request('core_user_get_users_by_field', $params, array('encoding' => 'UTF-8'));
    // var_dump($request);  // In case you want to see XML.

    $context = stream_context_create(array(
      'http' => array(
        'method' => "POST",
        'header' => "Content-Type: text/xml",
        'content' => $request
      )
    ));
    $path = $this->server . $this->dir . "/webservice/xmlrpc/server.php?wstoken=" . $this->token;
    // Send XML to server and get a reply from it.
    $file = file_get_contents($path, FALSE, $context); // $file is the reply from server.
    // Decode the reply.
    $response = xmlrpc_decode($file, 'UTF-8');

    // Note: lack of permissions on Moodle will get us an XML-formatted response with NULL values.
    // In other words, one must be absolutely sure to give all the required capabilities to web services account
    // in order to execute this function successfully. Moodle says that we need the following:
    // moodle/user:viewdetails, moodle/user:viewhiddendetails, moodle/course:useremail, moodle/user:update
    // for core_user_get_users_by_id call.
    // Handle errors.
    if (empty($response) || !is_array($response) || !is_array($response[0]) || !array_key_exists('id', $response[0])) {
      // We have an error.
      if (!empty($response) && $response['faultCode']) {
        $this->error = 'Moodle error: ' . $response['faultString'] . ". Fault code: " . $response['faultCode'] . ".";
      }
      else {
        $this->error = 'Moodle returned no info. Check if user id exists and whether the web service account has capabilities required to execute core_user_get_users_by_field call.';
      }
      $this->error .= " Actual reply from server: " . $file;
      return FALSE;
    }

    // This is our normal exit (returning an array of user properties).
    $user = $response[0];
    return $user;
  }

  // The createUser function tries to create a new Moodle user.
  function createUser($fields) {
    // Clear last error.
    $this->error = NULL;

    // Create XML for the request. XML must be set properly for this to work.
    $users = array($fields);
    $params = array($users);
    $request = xmlrpc_encode_request('core_user_create_users', $params, array(
      'escaping' => 'markup',
      'encoding' => 'UTF-8'
    ));
    // var_dump($request);  // In case you want to see XML.

    $context = stream_context_create(array(
      'http' => array(
        'method' => "POST",
        'header' => "Content-Type: text/xml",
        'content' => $request
      )
    ));

    $path = $this->server . $this->dir . "/webservice/xmlrpc/server.php?wstoken=" . $this->token;
    // Send XML to server and get a reply from it.
    $file = file_get_contents($path, FALSE, $context); // $file is the reply from server.
    // Decode the reply.
    $response = xmlrpc_decode($file, 'UTF-8');

    // Note: lack of permissions on Moodle will get us an error.
    // moodle/user:create capability is required for web service account to call core_user_create_users.
    // Handle errors.
    //if (empty($response) || !is_array($response) || !is_array($response[0]) || !array_key_exists('id', $response[0])) {
    if (!is_array($response) || isset($response['faultCode'])) {
      // We have an error.
      if (isset($response['faultCode'])) {
        $this->error = 'Moodle error: ' . $response['faultString'] . ". Fault code: " . $response['faultCode'] . ".";
      }
      else {
        $this->error = 'Moodle returned no info. Check if Moodle is set up properly (see readme).';
      }
      $this->error .= " Actual reply from server: " . $file;
      return FALSE;
    }

    // This is our normal exit. Returning a 2-member array with new user id and username.
    $user = $response[0];
    return $user;
  }

  // The updateUser function tries to update an existing Moodle user based on its idnumber.
  function updateUser($fields) {
    // Clear last error.
    $this->error = NULL;

    // Check if user exists.
    $user = $this->getUser($fields['idnumber']);
    if (!$user) {
      return FALSE;
    }

    $fields['id'] = $user['id'];

    // Create XML for the request. XML must be set properly for this to work.
    $users = array($fields);
    $params = array($users);
    $request = xmlrpc_encode_request('core_user_update_users', $params, array(
      'escaping' => 'markup',
      'encoding' => 'UTF-8'
    ));
    // var_dump($request);  // In case you want to see XML.

    $context = stream_context_create(array(
      'http' => array(
        'method' => "POST",
        'header' => "Content-Type: text/xml",
        'content' => $request
      )
    ));

    $path = $this->server . $this->dir . "/webservice/xmlrpc/server.php?wstoken=" . $this->token;
    // Send XML to server and get a reply from it.
    $file = file_get_contents($path, FALSE, $context); // $file is the reply from server.
    // Decode the reply.
    $response = xmlrpc_decode($file, 'UTF-8');

    // Note: lack of permissions on Moodle will get us an error.
    // moodle/user:update capability is required for web service account to call core_user_update_users.

    if ($response && xmlrpc_is_fault($response)) {
      dpm($response);
      $this->error = 'Moodle error: ' . $response['faultString'] . ". Fault code: " . $response['faultCode'] . ".";
      $this->error .= " Actual reply from server: " . $file;
      return FALSE;
    }

    // This is our normal exit after a successful update.
    return TRUE;
  }

  // The deleteUser function tries to delete an existing Moodle user based on its idnumber.
  function deleteUser($idnumber) {
    // Clear last error.
    $this->error = NULL;

    // Check if user exists.
    $user = $this->getUser($idnumber);
    if (!$user) {
      return FALSE;
    }

    // Create XML for the request. XML must be set properly for this to work.
    $userids = array($user['id']);
    $params = array($userids);
    $request = xmlrpc_encode_request('core_user_delete_users', $params, array('encoding' => 'UTF-8'));
    // var_dump($request);  // In case you want to see XML.

    $context = stream_context_create(array(
      'http' => array(
        'method' => "POST",
        'header' => "Content-Type: text/xml",
        'content' => $request
      )
    ));

    $path = $this->server . $this->dir . "/webservice/xmlrpc/server.php?wstoken=" . $this->token;
    // Send XML to server and get a reply from it.
    $file = file_get_contents($path, FALSE, $context); // $file is the reply from server.
    // Decode the reply.
    $response = xmlrpc_decode($file, 'UTF-8');

    if ($response && xmlrpc_is_fault($response)) {
      $this->error = "Moodle error: " . $response['faultString'] . " Fault code: " . $response['faultCode'];
      return FALSE;
    }

    // This is our normal exit after a successful delete.
    return TRUE;
  }

  // The getCourse function obtains information for a Moodle course identified by its id.
  function getCourse($id) {
    // Clear last error.
    $this->error = NULL;

    // Create XML for the request. XML must be set properly for this to work.
    $courseids = array($id);
    // $params = array('options'=>array('ids'=>$courseids)); // This does not work, return an exception inside Moodle.
    $params = array(array('ids' => $courseids)); // This works.
    $request = xmlrpc_encode_request('core_course_get_courses', $params, array('encoding' => 'UTF-8'));
    // var_dump($request);  // In case you want to see XML.

    $context = stream_context_create(array(
      'http' => array(
        'method' => "POST",
        'header' => "Content-Type: text/xml",
        'content' => $request
      )
    ));

    $path = $this->server . $this->dir . "/webservice/xmlrpc/server.php?wstoken=" . $this->token;
    // Send XML to server and get a reply from it.
    $file = file_get_contents($path, FALSE, $context); // $file is the reply from server.
    // Decode the reply.
    $response = xmlrpc_decode($file, 'UTF-8');

    // Note: lack of permissions on Moodle will get us an error.
    // Required capabilities for core_course_get_courses call:
    // moodle/course:view,moodle/course:update,moodle/course:viewhiddencourses
    // Make sure that your web service account role has those.
    // Handle errors.
    if (!is_array($response) || !is_array($response[0]) || !array_key_exists('id', $response[0])) {
      // We have an error.
      if ($response[faultCode]) {
        $this->error = 'Moodle error: ' . $response['faultString'] . ". Fault code: " . $response['faultCode'] . ".";
      }
      else {
        $this->error = "Moodle returned no info. Check if course id exists and whether the web service
          account has capabilities required to execute core_course_get_courses call.";
      }
      $this->error .= " Actual reply from server: " . $file;
      return FALSE;
    }

    // This is our normal exit (returning an array of course properties).
    $course = $response[0];
    return $course;
  }

  // The enrollUser function tries to enroll user in a course as a student.
  function enrollUser($idnumber, $course_id) {
    // Clear last error.
    $this->error = NULL;

    // Check whether user exists.
    $user = $this->getUser($idnumber);
    if (!$user) {
      return FALSE;
    }

    // Here, you may wish to check $user['enrolledcourses'] to see if a user is already enrolled in a course.
    // ...
    // Check whether course exists.
    $course = $this->getCourse($course_id);
    if (!$course) {
      return FALSE;
    }

    // Create XML for the request. XML must be set properly for this to work.  This format was hard to figure out.
    // I needed to debug the server code so see why method signatures did not match.
    $rid = ($role = user_role_load_by_name('student')) ? $role->rid : NULL;
    $enrolment = array(
      'roleid' => $rid,
      'userid' => $user['id'],
      'courseid' => $course_id
    );
    $enrolments = array($enrolment);
    $params = array($enrolments);
    $request = xmlrpc_encode_request('enrol_manual_enrol_users', $params, array('encoding' => 'UTF-8'));
    // var_dump($request);  // In case you want to see XML.

    $context = stream_context_create(array(
      'http' => array(
        'method' => "POST",
        'header' => "Content-Type: text/xml",
        'content' => $request
      )
    ));

    $path = $this->server . $this->dir . "/webservice/xmlrpc/server.php?wstoken=" . $this->token;
    // Send XML to server and get a reply from it.
    $file = file_get_contents($path, FALSE, $context); // $file is the reply from server.
    // Decode the reply.
    $response = xmlrpc_decode($file, 'UTF-8');

    // enrol/manual:enrol capability is required for the web services account.
    // Also, the account must be abble to assign the "Student" role - this is configured in
    // Site administration - Users - Permissions - Define roles - Allow role assignments (make sure that the "Student" role
    // is checked for Web Services Users category (this is my custom role for web services account).

    if ($response && xmlrpc_is_fault($response)) {
      $this->error = "Moodle error: " . $response['faultString'] . " Fault code: " . $response['faultCode'];
      return FALSE;
    }

    // Here, you may wish to check $user['enrolledcourses'] to see if a user gor enrolled, just to be safe.
    // $user = $this->getUser($user_id);
    // This is our normal exit after a successful enrollment.
    return TRUE;
  }

  // The enrollUser function tries to enroll user in a group.
  function enrollGroupMember($idnumber, $group_id) {
    // Clear last error.
    $this->error = NULL;

    // Check whether user exists.
    $user = $this->getUser($idnumber);
    if (!$user) {
      return FALSE;
    }

    // Here, you may wish to check $user['enrolledcourses'] to see if a user is already enrolled in a course.
    // ...
    // Check whether group exists.
    $group = $this->getCourseGroup($group_id);
    if (!$group) {
      return FALSE;
    }

    // Create XML for the request. XML must be set properly for this to work.
    $member = array('groupid' => $group_id, 'userid' => $user['id']);
    $members = array($member);
    $params = array($members);
    $request = xmlrpc_encode_request('core_group_add_group_members', $params, array('encoding' => 'UTF-8'));
    // var_dump($request);  // In case you want to see XML.

    $context = stream_context_create(array(
      'http' => array(
        'method' => "POST",
        'header' => "Content-Type: text/xml",
        'content' => $request
      )
    ));

    $path = $this->server . $this->dir . "/webservice/xmlrpc/server.php?wstoken=" . $this->token;
    // Send XML to server and get a reply from it.
    $file = file_get_contents($path, FALSE, $context); // $file is the reply from server.
    // Decode the reply.
    $response = xmlrpc_decode($file, 'UTF-8');

    // enrol/manual:enrol capability is required for the web services account.
    // Also, the account must be abble to assign the "Student" role - this is configured in
    // Site administration - Users - Permissions - Define roles - Allow role assignments (make sure that the "Student" role
    // is checked for Web Services Users category (this is my custom role for web services account).
    // Handle errors.
    if (isset($response['faultCode'])) {
      $this->error = 'Moodle error: ' . $response['faultString'] . ". Fault code: " . $response['faultCode'] . ".";
      return FALSE;
    }

    // This is our normal exit after a successful enrollment.
    return TRUE;
  }

  // The getCourseGroups function obtains information for all Moodle course groups identified by its id.
  function getCourseGroups($id) {
    // Clear last error.
    $this->error = NULL;

    // Create XML for the request. XML must be set properly for this to work.
    $courseid = intval($id);
    $params = array($courseid);
    $request = xmlrpc_encode_request('core_group_get_course_groups', $params, array('encoding' => 'UTF-8'));
    // var_dump($request);  // In case you want to see XML.
    $context = stream_context_create(array(
      'http' => array(
        'method' => "POST",
        'header' => "Content-Type: text/xml",
        'content' => $request
      )
    ));

    $path = $this->server . $this->dir . "/webservice/xmlrpc/server.php?wstoken=" . $this->token;
    // Send XML to server and get a reply from it.
    $file = file_get_contents($path, FALSE, $context); // $file is the reply from server.
    // Decode the reply.
    $response = xmlrpc_decode($file, 'UTF-8');

    // Note: lack of permissions on Moodle will get us an error.
    // Required capabilities for core_course_get_courses call:
    // moodle/course:view,moodle/course:update,moodle/course:viewhiddencourses
    // Make sure that your web service account role has those.
    // Handle errors.
    if (isset($response['faultCode'])) {
      $this->error = 'Moodle error: ' . $response['faultString'] . ". Fault code: " . $response['faultCode'] . ".";
      return FALSE;
    }

    // This is our normal exit (returning an array of course groups properties).
    $groups = $response;
    return $groups;
  }

  // The getCourseGroup function obtains information for a course group identified by its id.
  function getCourseGroup($id) {
    // Clear last error.
    $this->error = NULL;

    // Create XML for the request. XML must be set properly for this to work.
    $group = intval($id);
    $groupids = array($group);
    $params = array($groupids);
    $request = xmlrpc_encode_request('core_group_get_groups', $params, array('encoding' => 'UTF-8'));
    // var_dump($request);  // In case you want to see XML.
    $context = stream_context_create(array(
      'http' => array(
        'method' => "POST",
        'header' => "Content-Type: text/xml",
        'content' => $request
      )
    ));

    $path = $this->server . $this->dir . "/webservice/xmlrpc/server.php?wstoken=" . $this->token;
    // Send XML to server and get a reply from it.
    $file = file_get_contents($path, FALSE, $context); // $file is the reply from server.
    // Decode the reply.
    $response = xmlrpc_decode($file, 'UTF-8');

    // Note: lack of permissions on Moodle will get us an error.
    // Required capabilities for core_course_get_courses call:
    // moodle/course:view,moodle/course:update,moodle/course:viewhiddencourses
    // Make sure that your web service account role has those.
    // Handle errors.
    if (isset($response['faultCode'])) {
      $this->error = 'Moodle error: ' . $response['faultString'] . ". Fault code: " . $response['faultCode'] . ".";
      return FALSE;
    }

    // This is our normal exit (returning an array of course groups properties).
    $group = $response;
    return $group;
  }

  // The getGroupMembers function obtains information for all Moodle course group members identified by its id.
  function getGroupMembers($id) {
    // Clear last error.
    $this->error = NULL;

    // Create XML for the request. XML must be set properly for this to work.
    $group = intval($id);
    $groupids = array($group);
    $params = array($groupids);
    $request = xmlrpc_encode_request('core_group_get_group_members', $params, array('encoding' => 'UTF-8'));
    // var_dump($request);  // In case you want to see XML.
    $context = stream_context_create(array(
      'http' => array(
        'method' => "POST",
        'header' => "Content-Type: text/xml",
        'content' => $request
      )
    ));

    $path = $this->server . $this->dir . "/webservice/xmlrpc/server.php?wstoken=" . $this->token;
    // Send XML to server and get a reply from it.
    $file = file_get_contents($path, FALSE, $context); // $file is the reply from server.
    // Decode the reply.
    $response = xmlrpc_decode($file, 'UTF-8');

    // Note: lack of permissions on Moodle will get us an error.
    // Required capabilities for core_course_get_courses call:
    // moodle/course:view,moodle/course:update,moodle/course:viewhiddencourses
    // Make sure that your web service account role has those.
    // Handle errors.
    if (isset($response['faultCode'])) {
      $this->error = 'Moodle error: ' . $response['faultString'] . ". Fault code: " . $response['faultCode'] . ".";
      return FALSE;
    }

    // This is our normal exit (returning an array of course group members properties).
    $members = $response;
    return $members;
  }
}
