#!/usr/bin/python3

from udjat import logger, agent

class SampleAgent(agent):
	def __init__(self,properties):
		print("Initializing sample agent")
		super().__init__(properties)
		#print(dir(properties))
		#print(properties.testparameter)
		print(properties.get('testparameter','default value'))

	def setup(self,properties):
		print("Setting up from properties")
		super().setup(properties)

def agent_factory(properties):
	#print(dir(agent))
	logger.info("Agent factory method was called")
	return SampleAgent(properties)
