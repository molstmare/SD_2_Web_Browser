<?php

$name = htmlspecialchars($_POST["form-username"]); 
$password = htmlspecialchars($_POST["form-password"]);

$myObj->name = $name;
$myObj->password = $password;

$myJSON = json_encode($myObj);

$descriptors = array(
    0 => array("pipe", "r"),
    1 => array("pipe", "w"),
    2 => array("pipe", "w")
);

$process = proc_open("./ipc", $descriptors, $pipes);
$returnedValue = NULL;

if(is_resource($process)){
    fwrite($pipes[0], $myJSON);
    fclose($pipes[0]);

    $returnedValue = stream_get_contents($pipes[1]);
    fclose($pipes[1]);
}

echo $returnedValue;

?>
 
