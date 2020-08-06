//
//  macsound.mm
//  TAMGUI
//
//  Created by Claude Roux on 09/08/13.
//  Copyright 2013 Maison. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>
#import "AudioToolbox/AudioToolbox.h"

bool createSoundId(const char* thepath,SystemSoundID* _soundId);
void cleanSoundId(SystemSoundID*);
void playSoundId(SystemSoundID*);
void addSoundFunction(SystemSoundID* soundid,AudioServicesSystemSoundCompletionProc completionCallback,void* data);
void removeSoundFunction(SystemSoundID* soundid);
void TerminateKifCode();

bool createSoundId(const char* thepath,SystemSoundID* _soundId) {
     NSString* soundPath=[NSString stringWithUTF8String:thepath];

     //NSString *soundPath = [[NSBundle mainBundle] pathForResource:nres ofType:@"mp3"];
     if ([[NSFileManager defaultManager] fileExistsAtPath:soundPath])
          {
               NSURL* soundPathURL = [NSURL fileURLWithPath:soundPath];
               AudioServicesCreateSystemSoundID((__bridge CFURLRef)soundPathURL, _soundId);
               return true;
          }
     return false;
}

void cleanSoundId(SystemSoundID* soundid) {
     AudioServicesDisposeSystemSoundID(*soundid);
}

void playSoundId(SystemSoundID* soundid) {
     AudioServicesPlaySystemSound(*soundid);
}

void addSoundFunction(SystemSoundID* soundid,AudioServicesSystemSoundCompletionProc completionCallback,void* data) {
     AudioServicesAddSystemSoundCompletion (*soundid,NULL,NULL,completionCallback,data);
}

void removeSoundFunction(SystemSoundID* soundid) {
     AudioServicesRemoveSystemSoundCompletion (*soundid);
}
