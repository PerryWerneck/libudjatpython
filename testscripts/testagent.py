#!/usr/bin/python3

from udjat import logger, agent

class SampleAgent(agent):
	def __init__(self,properties):
		print("Initializing sample agent")
		super().__init__(properties)
		print(properties.testparameter)

def agent_factory(properties):
	#print(dir(agent))
	logger.info("Agent factory method was called")
	agent = SampleAgent(properties)
