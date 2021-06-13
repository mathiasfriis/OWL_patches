////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 
 
 LICENSE:
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 */

/* created by the OWL team 2013 */

////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ___StompBoxTemplate_hpp__
#define __StompBoxTemplate_hpp__

class StompBoxTemplate : public Patch {
private:
    bool buttonPressed, buttonPressed_state, ExpressionPedalTriggered, ExpressionPedalTriggered_state;
    bool isButtonTriggered();
    int isExpressionPedalTriggered();
    virtual void onButtonPressed();
    virtual void onButtonReleased();
    virtual void onExpressionPedalPressed();
    virtual void onExpressionPedalReleased();
    virtual void processAudioLoop(float* buf_L, float* buf_L, int buffer_size);

public:
  StompBoxTemplate(){
    ExpressionPedalPressed=false;
    ExpressionPedalPressed_state=false;
    buttonState = false;
  }

  ~StompBoxTemplate() {
    }

  // Check if push button was triggered
  // Returns 0 if state is unaltered since last check
  // Returns 1 if button was just pushed down
  // Returns -1 if button was just released
  int isButtonTriggered(){
    // Get current state
    buttonPressed = isButtonPressed(PUSHBUTTON);

    // Compare to old state, to see wheter the pedal was just triggered
    int return_val;
    if((buttonPressed==true) && (buttonPressed_state==false))
    {
        return_val = 1;
    }
    else if ((buttonPressed==false) && (buttonPressed_state==true))
    {
        return_val = -1;
    }
    else
    {
        return_val = 0;
    }

    // Write to state variable
    buttonPressed_state=buttonPressed;

    return return_val;
  }

   // Check if expression pedal input was triggered (i.e. binary expression pedal value)
  // Returns 0 if state is unaltered since last check
  // Returns 1 if pedal was just triggered
  // Returns -1 if pedal was just released
  int isExpressionPedalTriggered(){

    // Get current state based on parameter input value
    if(getParameterValue(PARAMETER_E)<0.05)
    {
        ExpressionPedalPressed=true;
    }
    else
    {
        ExpressionPedalPressed=false;
    }

    // Compare to old state, to see wheter the pedal was just triggered
    int return_val;
    if((ExpressionPedalPressed==true) && (ExpressionPedalPressed_state==false))
    {
        return_val = 1;
    }
    else if ((ExpressionPedalPressed==false) && (ExpressionPedalPressed_state==true))
    {
        return_val = -1;
    }
    else
    {
        return_val = 0;
    }

    // Write to state variable
    ExpressionPedalPressed_state=ExpressionPedalPressed;

    return return_val;
  }
 

  void processAudio(AudioBuffer &buffer){
    int buffer_size = buffer.getSize();

    float* buf_L = buffer.getSamples(0);
    float* buf_R = buffer.getSamples(1);

    int wasButtonTriggered = isButtonTriggered();
    int wasExpressionPedalTriggered = isExpressionPedalTriggered();

    if wasButtonTriggered == 1
    {
         onButtonPressed();
    }
    else if wasButtonTriggered == -1
    {
        onButtonReleased();
    }

    if wasExpressionPedalTriggered == 1
    {
         onExpressionPedalPressed();
    }
    else if wasExpressionPedalTriggered == -1
    {
        onExpressionPedalReleased();
    }

    processAudioLoop(float* buf_L, float* buf_R, int buffer_size);

  }
};


#endif /* __FlangerTestPatch_hpp__ */