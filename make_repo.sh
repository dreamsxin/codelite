################################################################################
## This file is part of CodeLite IDE and is released 
## under the terms of the GNU General Public License as published by  
##    the Free Software Foundation; either version 2 of the License, or     
##    (at your option) any later version.                              
################################################################################


#########################################################################################################################
##1. Install Apache and create the folder debs under its htdocs directory
##2. Copy the .deb file to /usr/local/apache2/htdocs/debs
##3. Run the following command from /usr/local/apache2/htdocs/debs: 
##	/usr/bin/dpkg-scanpackages . /dev/null > Packages
##4. Compress the Packages file to dists/gutsy/restricted/binary-i386/Packages.gz
##5. Add the following entry to your /etc/apt/sources.list: deb http://eran-linux:/debs  gutsy restricted (replace eran-linux with your IP/server name)
echo "removing old deb files..."
rm -fr *.deb
rm /var/www/debs/*.deb
echo "Creating new deb file..."
./make_deb.sh
echo "Copying deb to /usr/local/apache2/htdocs/debs/"
cp *.deb /var/www/debs
echo "changing directory to /var/www/debs/"
cd /var/www/debs
echo "Running: /usr/bin/dpkg-scanpackages tool..."
/usr/bin/dpkg-scanpackages . /dev/null > Packages
echo "Compressing Packages..."
gzip Packages
echo "Moving Packages.gz to dist directory"
mv Packages.gz dists/gutsy/restricted/binary-i386/Packages.gz
echo "Done"

