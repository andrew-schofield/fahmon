<?php
 if (isset($rem)) { 
     setcookie ("blogKo_name", $comm_name,time()+1209600); 
     setcookie ("blogKo_mail", $comm_e_mail,time()+1209600); 
     setcookie ("blogKo_www", $comm_website,time()+1209600); 
     } 
 if (!isset($button)) { 
     $comm_name = $_COOKIE["blogKo_name"]; 
     $comm_e_mail = $_COOKIE["blogKo_mail"]; 
     $comm_website = $_COOKIE["blogKo_www"]; 
     }
     session_start();
?>
<?php
echo("<?xml ");
echo("version=\"1.0\" encoding=\"utf-8\"?>");
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en">
<head>
<meta http-equiv="Content-Type" content="application/php; charset=utf-8" />
<meta name="generator" content="NanoBlogger 3.3" />
<meta name="robots" content="all" />
<meta name="revisit-after" content="31 days" />

<title>January  25, 2008 Archives | FahMon News</title>

<link rel="stylesheet" href="../../../../styles/nb_fahmon.css" type="text/css" media="all" />
<link rel="stylesheet" href="../../../../styles/print.css" type="text/css" media="print" />
<link rel="stylesheet" href="../../../../blogkomm/blogkomm.css" type="text/css" />
<link rel="shortcut icon" href="../../../../images/favicon.ico" />
	<!--[if lt IE 7]>
	<style>
	.TopNavigation ul
{
	margin-left:100px;
}
	</style>
	<![endif]-->
<link rel="alternate" type="application/atom+xml"
title="Atom 1.0"
href="../../../../atom.xml"
/>
<link rel="alternate" type="application/rss+xml"
title="RSS 2.0"
href="../../../../rss.xml"
/>

</head>

<body>
<div id="BodyWrapper">
<div id="MainContainer">
<div class="TopNavigation">
<ul>
<li><a href="http://fahmon.net">Home</a></li>
<li><a href="../../../..//../../download.html">Download</a></li>
<li><a href="../../../..//../../screenshots.html">Screenshots</a></li>
<li><a href="../../../..//../../about.html">About</a></li>
<li class="Selected"><a href="../../../..//../../news/">News</a></li>
<li><a href="http://trac.fahmon.net">Support &amp; Development</a></li>
</ul>
</div>
<br />
<div class="box_full">
<div id="main">
<div id="container" class="clearfix">
	<div id="header" class="clearfix">
		<h1><a href="../../../../index.php" accesskey="1">FahMon News</a></h1>
		<span class="description">News and development of FahMon</span>
	</div>
	<div id="altcontent">
		<div id="wrap">
			<a id="top"></a>
			<h2>January  25, 2008 Archives</h2>
			<div id="menu">
				<a href="../../../../index.php">Main</a> &#62;
				<a href="../../../../archives/index.php">Archives</a> &#62;
				<a href="../../../../archives/2008/index.php">2008</a> &#62;
				<a href="../../../../archives/2008/01/index.php">01</a> &#62;
				<br />
<a href="../../../../archives/2008/01/24/index.php">&#60;</a>
				25
<a href="../../../../archives/2008/01/26/index.php">&#62;</a>
			</div>
			<div class="archives">
				<div class="item">
	<a id="e2008-01-25T12_54_12.txt"></a>
	<h2 class="date">Fri Jan 25 12:54:12 CST 2008</h2>
	<div class="blogbody">
		<h3 class="title">Known bugs in 2.3.2beta</h3>
		<div class="item-description">
			There have been a couple of reports of undocumented bugs in the 2.3.2beta release which have now been 
fixed in SVN. It is likely that a beta2 will be released in the near future to check that these bugs are 
no longer present.
<ul>
<li>FahMon will crash if the project database doesn't contain project info for a WU it is monitoring.
<li>The simple web output erroneously adds an extra "points" string to the credit field.
<li>Several text items on all web output files cannot be translated.
</ul>
		</div>
	</div>

	<div class="posted">
		<br />Posted by <span class="item-creator">Andrew Schofield</span>
| <a class="link" href="../../../../archives/2008/01/25/index.php#e2008-01-25T12_54_12.txt">Permanent Link</a>
| Categories: <!-- Development --><a href="../../../../archives/development/index.php">Development</a>
<!--<a href="../../../../../cgi-bin/cgicomment.cgi?article=archives/2008/01/25/index.php#e2008-01-25T12_54_12.txt#comments" target="_blank" >Comments</a>//-->
<?php
 $post_url=parse_url('http://fahmon.net/news/archives/2008/01/25/index.php#e2008-01-25T12_54_12.txt', PHP_URL_PATH); 
 $blog_id='e2008-01-25T12_54_12.txt';
 $blog_mail='andrew_s@fahmon.net';
 define (BK_PATH, "/home/salamander/webapps/fahmon/news/blogkomm");
 include (BK_PATH."/module/blogkomm_show_link.php");
?>
	</div>
</div>
			</div>
		</div>
	</div>
</div>
<div id="footer" style="clear:both;">
	<div class="menu">
		<a href="#top">&#47;&#92;</a><br />
	</div>
</div>
</div>
</div>
</div>
</div>
</body>
</html>