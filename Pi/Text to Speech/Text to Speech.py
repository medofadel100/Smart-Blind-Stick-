from gtts import gTTS 

import vlc

# import Os module to start the audio file
import os 
  
mytext = 'temperature is high please stop and check cooling system'
  
# Language we want to use 
language = 'en'
  

myobj = gTTS(text=mytext, lang=language, slow=False) 
  

myobj.save("temp.mp3") 
  
# Play the converted file 
#os.system("start temp.mp3")

media = vlc.MediaPlayer("temp.mp3")
 
# start playing video
media.play()
