# IntegrityLevel

Bare bones code sample that demonstrates how to spin up a process in Windows at a lower integrity level. This is the same strategy used by Edge/Chrome/IE/Office/Acrobat - processes handing dodgy data are constrained to minimize damage if a process blows up while parsing untrusted data. 

A simple test is to run this code, it'll spin up a low-integrity copy of Notepad. Now try to save the contents of Notepad to your desktop. Even though your account has full access to your own desktop, you'll get an Access Denied because the low-integrity Notepad is trying to write to your medium-integrity desktop, and the integrity check is performed the normal DACL check. You can't "write-up" so the write fails.

