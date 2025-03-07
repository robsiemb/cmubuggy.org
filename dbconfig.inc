<?php
  $config_json_str = file_get_contents($_SERVER['DOCUMENT_ROOT'].'/dbconfig.json');
  $config = json_decode($config_json_str);

  $dbhost = $config->DB_HOST;
  $dbuser = $config->DB_USER;
  $dbpass = $config->DB_PASSWORD;

  $HISTORY_DATABASE = $config->DB_NAME;  // '[database name for history data]'

  $ENABLE_GA_TAG = $config->GA_TAG;  // false or unset will prevent GA tag from showing
?>
