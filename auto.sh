# Automation file
# Step 1. File conversion from .md to .html
FILES=*.md
for f in $FILES
do
  # extension="${f##*.}"
  filename="${f%.*}"
  echo "Converting $f to $filename.html"
  `pandoc -s -c base.css $f -t html -o $filename.html`
done
# Step 2. Uploading everything to github
# If there is commit message then upload
if [ "$#" -gt 0 ]
then
    cd .. # Now in students folder
    cd .. # Now in lab folder
    git pull
    git add --all
    git commit -m "$*"
    git push
    git ftp push -u hola@fablabsitges.org -P ftp://beachlab.org/public_html/htgaa # Uncomment this line to upload to ftp. Replace <user> and server to your settings
else
    echo "Not uploading (Empty commit message)"
fi
