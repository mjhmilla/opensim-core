#ifndef _Spindle_h_
#define _Spindle_h_
/* -------------------------------------------------------------------------- *
 *                   OpenSim:  StretchController.h                    *
 * -------------------------------------------------------------------------- *
 * The OpenSim API is a toolkit for musculoskeletal modeling and simulation.  *
 * See http://opensim.stanford.edu and the NOTICE file for more information.  *
 * OpenSim is developed at Stanford University and supported by the US        *
 * National Institutes of Health (U54 GM072970, R24 HD065690) and by DARPA    *
 * through the Warrior Web program.                                           *
 *                                                                            *
 * Copyright (c) 2005-2016 Stanford University and the Authors                *
 * Author(s): Chris Dembia, Shrinidhi K. Lakshmikanth, Ajay Seth,             *
 *            Thomas Uchida                                                   *
 *                                                                            *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may    *
 * not use this file except in compliance with the License. You may obtain a  *
 * copy of the License at http://www.apache.org/licenses/LICENSE-2.0.         *
 *                                                                            *
 * Unless required by applicable law or agreed to in writing, software        *
 * distributed under the License is distributed on an "AS IS" BASIS,          *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   *
 * See the License for the specific language governing permissions and        *
 * limitations under the License.                                             *
 * -------------------------------------------------------------------------- */


#include <OpenSim/OpenSim.h>

using namespace std;
namespace OpenSim {

//------------------------------------------------------------------------------
// Spindle
//------------------------------------------------------------------------------
class Spindle : public ModelComponent {
	OpenSim_DECLARE_CONCRETE_OBJECT(Spindle, ModelComponent);

public:
	// Property of the controller for the input signal delay 
	OpenSim_DECLARE_PROPERTY(delay_time, double, 
		"Duration of delay (seconds).");

	// TODO noise, other properties

	// Not using connector so that we can add Spindle to non-Muscle 
	// components, e.g. PathActuator

	//// Connector to the Muscle for which the spindle is measuring fiber length
	//OpenSim_DECLARE_CONNECTOR(muscle, Muscle,
	//	"The muscle for which the spindle is measuring fiber length");

	// Input the length to use in the stretch controller
	OpenSim_DECLARE_INPUT(fiberLength, double, SimTK::Stage::Model,
		"The input fiber length measured from the muscle");

	// Output the control signal generated by the controller.
	OpenSim_DECLARE_OUTPUT(length, double, getLength, SimTK::Stage::Position);

	Spindle() {
		constructProperties();
	}

	// Member function for getting the delayed length
	double getLength(const SimTK::State& s) const
	{
		const Delay& lengthDelay = getMemberSubcomponent<Delay>(_delayIdx);
		//double length = getInputValue<double>(s, "length");
		double length = lengthDelay.getValue(s);
		return length;
	}


private:
	void constructProperties() {
		constructProperty_delay_time(0.0);
	}

	void extendFinalizeFromProperties() override {
		Super::extendFinalizeFromProperties();
		Delay& lengthDelay = updMemberSubcomponent<Delay>(_delayIdx);
		lengthDelay.set_delay(get_delay_time());
	}
	
	void extendConnectToModel(Model& model) override {
		Super::extendConnectToModel(model);

		// Set the input for the Delay component.
		Delay& lengthDelay = updMemberSubcomponent<Delay>(_delayIdx);
		const Input<double>& in = getInput<double>("fiberLength");
		const Output<double>::Channel& out = in.getChannel();
		lengthDelay.updInput("input").connect(out, "length_out");
	}

	MemberSubcomponentIndex _delayIdx
		{ constructSubcomponent<Delay>("length_delay") };

}; // end of Spindle

} // end of namespace OpenSim

#endif // _Spindle_h_
