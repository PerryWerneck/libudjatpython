#!/usr/bin/python3

from udjat import Agent, Action, State

class SampleAgent(Agent):

	#
	# Initialize agent, set custom properties and value.
	# (The 'properties' argument is used to acess the definitions from XML file)
	#
	def __init__(self,properties):

		# Call parent object to load XML definitions.
		super().__init__(properties)

		print('---------------------------------')

		# Set custom properties
		# Override the properties set by xml definition
		self.name = "SampleAgent"
		self.info("Setting up from properties")
		self.summary = "A really simple python agent"
		self.label = "Sample"

		# Set initial agent value
		# (Every time the value changes a new state will be computed).
		self.value = 0

		# Just for debugging, show the computed state.
		st = self.state	
		print('state: label={} summary={} icon={} level={}'.format(
			st.label,
			st.summary,
			st.icon,
			st.level
		))
	
	#
	# Update agent value, called at regular intervals from main controller
	# based on the attribute update-timer on XML definition.
	#
	def refresh(self,ondemand):

		self.info("-----> Updating agent value")

		# Update value, compute a new state.
		self.value = self.value + 1

		if self.value == 1:
			# Trigger a customized alert
			# (The alert arguments depends on the alert type)
			self.alert(
				type='script',
				cmdline='echo A bash based alert was triggered from python'
			)

		if self.value == 4:

			# Set a customized state for value 4 overriding the one defined by xml.
			self.state = {
				'name': 'v3',
				'level': 'warning',
				'label': 'Sample state',
				'summary': 'The state value is 4 - Set by python code',
				'body': 'This is the body for the current state, show a more detailed info',
				'url': 'https://google.com',
			}


		if self.value == 5:

			# Another way to set a customized state.
			self.state = State(
				name= 'v5',
				level = 'error',
				label = 'Sample state',
				summary = 'The state value is 5 - Set by python code',
				body= 'This is the body for the current state, show a more detailed info',
				url= 'https://google.com',
			)

		return True

#	def call(self,request,response):
#		return super().call(request,response)

class SampleAction(Action):
	def __init__(self,properties):
		super().__init__(properties)

	def call(self,request,response):
		self.info("Python action was activated")
		return 0 # super().call(request,response)

def AgentFactory(properties):
	return SampleAgent(properties)

def ActionFactory(properties):
	return SampleAction(properties)
