<!DOCTYPE HTML>

<html>

<head>
    <meta charset=UTF-8>
    <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css">
    <link rel="stylesheet" href="style.css">
    <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.4.0/jquery.min.js"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.7/umd/popper.min.js"></script>
    <script src="https://maxcdn.bootstrapcdn.com/bootstrap/4.3.1/js/bootstrap.min.js"></script>
	<title>Balcão Virtual</title>
</head>

<body>
    <nav class="nav navbar-expand-sm fixed-top">
      <ul>
          <p><b>Balcão Virtual</b></p>
      </ul>
    </nav>

<?php
error_reporting(0);

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
</body>

</html>
