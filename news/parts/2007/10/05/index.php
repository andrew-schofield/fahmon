<div class="item">
	<a id="e2007-10-05T09_44_45.txt"></a>
	<h2 class="date">Fri Oct  5 09:44:45 CDT 2007</h2>
	<div class="blogbody">
		<h3 class="title">First post for translators</h3>
		<div class="item-description">
			<p>For the existing translators out there, and for any potential new 
translators, this is the section you want to be watching to stay up 
to date with when new translations are required.</p>
<p>This generally means you'll get a few days advance on everyone 
else for downloading the official release, however it won't be 
officially supported, and should really only be used for checking 
that your new translations work etc.</p>
<p>To subscribe to this website category, the best way is to go to 
the translations category link on the side bar and subscribe to the 
rss feed. Alternatively you can click on the rss/atom feeds on the 
"syndicate" panel on the right, which any modern browser will allow 
you to bookmark, or send to google reader.</p>
<p>To get involved in translations right away, all you need to 
download is the fahmon.pot file from the svn trunk (see the home 
page for the direct link). About a week or so prior to an official 
release, a new post will be made here to notify you of the 
prerelease translation source (if you are keen and keep up with 
development, you shouldn't have to do very much on the prerelease 
versions).</p>
<p>To submit your translations, at the moment the best way to do it 
is the submit a ticket on trac, and attach the .po file you have 
created. I can then merge that into the source.</p>
		</div>
	</div>

	<div class="posted">
		<br />Posted by <span class="item-creator">Andrew Schofield</span>
| <a class="link" href="../../../../archives/2007/10/05/index.php#e2007-10-05T09_44_45.txt">Permanent Link</a>
| Categories: <!-- Translations --><a href="../../../../archives/translations/index.php">Translations</a>
<!--<a href="../../../../../cgi-bin/cgicomment.cgi?article=archives/2007/10/05/index.php#e2007-10-05T09_44_45.txt#comments" target="_blank" >Comments</a>//-->
<?php
 $post_url=parse_url('http://fahmon.net/news/archives/2007/10/05/index.php#e2007-10-05T09_44_45.txt', PHP_URL_PATH); 
 $blog_id='e2007-10-05T09_44_45.txt';
 $blog_mail='andrew_s@fahmon.net';
 define (BK_PATH, "/home/salamander/webapps/fahmon/news/blogkomm");
 include (BK_PATH."/module/blogkomm_show_link.php");
?>
	</div>
</div>
<div class="item">
	<a id="e2007-10-05T09_08_52.txt"></a>
	<h2 class="date">Fri Oct  5 09:08:52 CDT 2007</h2>
	<div class="blogbody">
		<h3 class="title">Latest development release</h3>
		<div class="item-description">
			<p>For those of you interested in downloading bleeding edge code, 
svn revision r38 is reasonably stable and adds a few new features in 
addition to a number of bug-fixes</p>
<p>To download this specific revisions, you can either download the 
snapshot as a zip file from <a 
href="http://trac.fahmon.net/changeset/38/trunk?old_path=%2F&amp;format=zip">here</a> or you can export it from the svn repository using the following command (you should be able to do this anonymously):</p>
<pre>svn export -r38 http://svn.fahmon.net/trunk fahmon</pre>
<p>This revision contains the following changes:</p>
<pre>
* Updated and improved documentation (this is still "Work
  in Progress").
* Added Czech translation.
* Preferences dialog and client list now save changes
  immediately, which should help in situations where FahMon
  crashes, like fast-user-switching or closing RDP/VNC
  sessions.
* Added Drag-and-Drop function to the client list to allow
  new clients to be added more easily.
* Add Gromacs SMP CVS (GROCVS) core support.
* Adjusted margin on the benchmarks dialog.
* Fixed bug in "Effective Duration" code that produced
  incorrect results when calculating values for WUs with
  less than 100 frames
* Detailed instructions on how to compile fahmon using the
  MS Free Tools are now provided.
* Altered progress detection method to use FAHlog.txt, this
  should help with cores that don't update unitinfo.txt in
  a timely fashion, or at all. Unitinfo.txt is still used
  as a fallback if 1) The projectId is unknown, 2) The WU
  is still in the "startup" phase (i.e. yellow).
* Added WebApp module. You can now export the monitoring
  status to 3 different formats, WebApp - a fancy jscript
  powered web page that mimics the FahMon interface; Simple
  Web - a simple web page containing useful data arranged
  in a simple table; Simple Text - similar to Simple Web,
  but optimised for shell based viewing - useful for
  monitoring over SSH.</pre>
		</div>
	</div>

	<div class="posted">
		<br />Posted by <span class="item-creator">Andrew Schofield</span>
| <a class="link" href="../../../../archives/2007/10/05/index.php#e2007-10-05T09_08_52.txt">Permanent Link</a>
| Categories: <!-- Development --><a href="../../../../archives/development/index.php">Development</a>
<!--<a href="../../../../../cgi-bin/cgicomment.cgi?article=archives/2007/10/05/index.php#e2007-10-05T09_08_52.txt#comments" target="_blank" >Comments</a>//-->
<?php
 $post_url=parse_url('http://fahmon.net/news/archives/2007/10/05/index.php#e2007-10-05T09_08_52.txt', PHP_URL_PATH); 
 $blog_id='e2007-10-05T09_08_52.txt';
 $blog_mail='andrew_s@fahmon.net';
 define (BK_PATH, "/home/salamander/webapps/fahmon/news/blogkomm");
 include (BK_PATH."/module/blogkomm_show_link.php");
?>
	</div>
</div>
<div class="item">
	<a id="e2007-10-05T07_22_55.txt"></a>
	<h2 class="date">Fri Oct  5 07:22:55 CDT 2007</h2>
	<div class="blogbody">
		<h3 class="title">New Website</h3>
		<div class="item-description">
			<p>FahMon now has a nice new website which you are reading right 
now.</p>
<p>As well as being <i>the</i> source for downloading FahMon, it now 
integrates with trac and svn for the development and support side 
too.</p>
<p>Hopefully this should be easier for you as end users too, and you 
should be able to syndicate the RSS/ATOM news feeds for new releases, 
development news and information important to translators.</p>
<p>Since trac is being used to handle the project management, we now 
have a sophisticated bug tracking and milestone management system. 
This makes it very easy for you to submit bug reports and for me to 
manage them, and the project as a whole.</p>
<p>I'm still working on the trac side of things, so it doesn't allow 
you to create tickets etc. yet, but that will come soon.</p>
<p>Anyone can now browse the development source, either though the 
trac browser, or via the svn server itself at svn.fahmon.net</p>
<p>If you want to download the most recent development source, you 
can check it out from svn using your favourite svn client</p>
<pre>svn export http://svn.fahmon.net/trunk fahmon</pre>
		</div>
	</div>

	<div class="posted">
		<br />Posted by <span class="item-creator">Andrew Schofield</span>
| <a class="link" href="../../../../archives/2007/10/05/index.php#e2007-10-05T07_22_55.txt">Permanent Link</a>
| Categories: <!-- Website --><a href="../../../../archives/website/index.php">Website</a>
<!--<a href="../../../../../cgi-bin/cgicomment.cgi?article=archives/2007/10/05/index.php#e2007-10-05T07_22_55.txt#comments" target="_blank" >Comments</a>//-->
<?php
 $post_url=parse_url('http://fahmon.net/news/archives/2007/10/05/index.php#e2007-10-05T07_22_55.txt', PHP_URL_PATH); 
 $blog_id='e2007-10-05T07_22_55.txt';
 $blog_mail='andrew_s@fahmon.net';
 define (BK_PATH, "/home/salamander/webapps/fahmon/news/blogkomm");
 include (BK_PATH."/module/blogkomm_show_link.php");
?>
	</div>
</div>