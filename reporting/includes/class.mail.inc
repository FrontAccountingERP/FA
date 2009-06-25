<?php
/*
    Name:           eMail
    Description:    Simple sending eMail in text and HTML with CC, BCC and attachment
    Version:        1.0
    last modified:  2004-05-14

    Autor:          Daniel Käfer
    Homepage:       http://www.danielkaefer.de

    Leave this header in this file!
*/

class email
{
    var $to = array();
    var $cc = array();
    var $bcc = array();
    var $attachment = array();
    var $boundary = "";
    var $header = "";
    var $subject = "";
    var $body = "";

    function email($name, $mail)
    {
        $this->boundary = md5(uniqid(time()));
        $this->header .= "From: $name <$mail>\n";
    }

    function to($mail)
    {
    	$this->to[] = $mail;
    }

    function cc($mail)
    {
    	$this->cc[] = $mail;
    }

    function bcc($mail)
    {
    	$this->bcc[] = $mail;
    }

    function attachment($file)
    {
		$this->attachment[] = $file;
    }

    function subject($subject)
    {
    	$this->subject = $subject;
    }

    function text($text)
    {
	    $this->body = "Content-Type: text/plain; charset=ISO-8859-1\n";
	    $this->body .= "Content-Transfer-Encoding: 8bit\n\n";
	    $this->body .= $text."\n";
    }

    function html($html)
    {
	    $this->body = "Content-Type: text/html; charset=ISO-8859-1\n";
	    $this->body .= "Content-Transfer-Encoding: quoted-printable\n\n";
	    $this->body .= "<html><body>\n".$html."\n</body></html>\n";
    }

	function mime_type($filename)
	{
		$file = basename($filename, '.zip');
		if ($filename == $file . '.zip') return 'application/x-zip-compressed';
		$file = basename($filename, '.pdf');
		if ($filename == $file . '.pdf') return 'application/pdf';
		$file = basename($filename, '.csv');
		if ($filename == $file . '.csv') return 'application/vnd.ms-excel';
		$file = basename($filename, '.tar');
		if ($filename == $file . '.tar') return 'application/x-tar';
		$file = basename($filename, '.tar.gz');
		if ($filename == $file . '.tar.gz') return 'application/x-tar-gz';
		$file = basename($filename, '.tgz');
		if ($filename == $file . '.tgz') return 'application/x-tar-gz';
		$file = basename($filename, '.gz');
		if ($filename == $file . '.gz') return 'application/x-gzip';
		return 'application/unknown';
	}

	function send()
    {
        // CC Empfänger hinzufügen
        $max = count($this->cc);
        if ($max > 0)
        {
            $this->header .= "Cc: ".$this->cc[0];
            for ($i = 1; $i < $max; $i++)
            {
                $this->header .= ", ".$this->cc[$i];
            }
            $this->header .= "\n";
        }
        // BCC Empfänger hinzufügen
        $max = count($this->bcc);
        if ($max > 0)
        {
            $this->header .= "Bcc: ".$this->bcc[0];
            for ($i = 1; $i < $max; $i++)
            {
                $this->header .= ", ".$this->bcc[$i];
            }
            $this->header .= "\n";
        }
        $this->header .= "MIME-Version: 1.0\n";
	    $this->header .= "Content-Type: multipart/mixed; boundary=$this->boundary\n\n";
	    $this->header .= "This is a multi-part message in MIME format\n";
        $this->header .= "--$this->boundary\n";
        $this->header .= $this->body;

        // Attachment hinzufügen
        $max = count($this->attachment);
        if ($max > 0)
        {
            for ($i = 0; $i < $max; $i++)
            {
                $file = fread(fopen($this->attachment[$i], "r"), filesize($this->attachment[$i]));
                $this->header .= "--".$this->boundary."\n";
                $this->header .= "Content-Type: " .$this->mime_type(basename($this->attachment[$i])). "; name=".basename($this->attachment[$i])."\n";
                $this->header .= "Content-Transfer-Encoding: base64\n";
                $this->header .= "Content-Disposition: attachment; filename=".basename($this->attachment[$i])."\n\n";
                $this->header .= chunk_split(base64_encode($file))."\n";
                $file = "";
            }
        }
        $this->header .= "--".$this->boundary."--\n\n";

        foreach($this->to as $mail)
        {
            $ret = mail($mail, $this->subject, "", $this->header);
        }
        return $ret;
    }
}
?>